#!/usr/bin/env python3

'''
/*
 * The Shadow Simulator
 * Copyright (c) 2010-2011, Rob Jansen
 * See LICENSE for licensing information
 */
'''

import sys, os, argparse, subprocess, multiprocessing, shlex, shutil, tarfile, gzip, stat, time
from datetime import datetime

import logging
logging.basicConfig(format='%(asctime)s [%(levelname)s] %(message)s', level=logging.INFO)

BUILD_PREFIX = "./build"
INSTALL_PREFIX = os.path.expanduser("~/.local")
HELP_STR = "Show this help message and exit."

def main():
    parser_main = argparse.ArgumentParser(
        description='Utility to help set up the Shadow simulator',
        add_help=False,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser_main.add_argument('-h', '--help', action='help', default=argparse.SUPPRESS, help=HELP_STR)

    # set up our commands
    subparsers_main = parser_main.add_subparsers(
        help='Run a subcommand (for help use <subcommand> --help).')

    # configure build subcommand
    parser_build = subparsers_main.add_parser('build',
        add_help=False,
        help='Configure and build Shadow.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser_build.set_defaults(func=build,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser_build.add_argument('-h', '--help', action='help', default=argparse.SUPPRESS, help=HELP_STR)

    # add building options
    parser_build.add_argument('-p', '--prefix',
        help="Configure PATH as Shadow root installation directory",
        metavar="PATH",
        action="store", dest="prefix",
        default=INSTALL_PREFIX)

    parser_build.add_argument('-s', '--search',
        # Ultimately sets CMAKE_PREFIX_PATH
        # https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html#variable:CMAKE_PREFIX_PATH
        help="Append PATH to the list of paths searched for libraries, includes, pkg-config entries etc. Useful if dependencies are installed to non-standard locations, or when compiling custom libraries.",
        metavar="PATH",
        action="append", dest="search_prefix",
        default=[])

    parser_build.add_argument('-c', '--clean',
        help="Force a full rebuild of Shadow by removing the build cache directory.",
        action="store_true", dest="do_force_rebuild",
        default=False)

    parser_build.add_argument('-g', '--debug',
        help="Build in extra memory checks and debugging symbols when running Shadow. Simulator performance will be significantly reduced.",
        action="store_true", dest="do_debug",
        default=False)

    parser_build.add_argument('--coverage',
        help="Enable profiling data for code coverage reporting.",
        action="store_true", dest="do_coverage",
        default=False)

    parser_build.add_argument('--use-perf-timers',
        help="Compile in timers for tracking the run time of various internal operations.",
        action="store_true", dest="do_use_perf_timers",
        default=False)

    parser_build.add_argument('-v', '--verbose',
        help="Print verbose output from the compiler.",
        action="store_true", dest="do_verbose",
        default=False)

    parser_build.add_argument('-j', '--jobs',
        help="Number of jobs to run simultaneously during the build.",
        metavar="N", type=int,
        action="store", dest="njobs",
        default=multiprocessing.cpu_count())

    parser_build.add_argument('-t', '--test',
        help="Build tests.",
        action="store_true", dest="do_test",
        default=False)

    parser_build.add_argument('--extra',
        help="Build extra tests (requires additional dependencies).",
        action="store_true", dest="do_extra_test",
        default=False)

    parser_build.add_argument('--werror',
        help="Turn compiler warnings into errors.",
        action="store_true", dest="do_werror",
        default=False)

    # configure test subcommand
    parser_test = subparsers_main.add_parser('test', add_help=False, help='Run Shadow tests.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser_test.set_defaults(func=test,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser_test.add_argument('-h', '--help', action='help', default=argparse.SUPPRESS, help=HELP_STR)

    parser_test.add_argument('testname_regex',
        help="Run only the tests that match this regex.", metavar="testname-regex", nargs="?")

    parser_test.add_argument('-j', '--jobs',
        help="Number of tests to run simultaneously during testing.",
        metavar="N", type=int,
        action="store", dest="njobs",
        # use a minimum of 4 jobs by default
        default=max(4, multiprocessing.cpu_count()))

    parser_test.add_argument('-r', '--rerun-failed',
        help="Run only the tests that failed previously.",
        action="store_true", dest="rerun_failed",
        default=False)

    # TODO: Ideally make the default more like 1s, and add more individual
    # exceptions as needed (using e.g. `set_tests_properties(slow_test_name
    # PROPERTIES TIMEOUT 30)`
    parser_test.add_argument('-t', '--timeout',
        help="Default test timeout, in seconds.",
        metavar="s", type=int,
        action="store", dest="timeout",
        default=20)

    parser_test.add_argument('-v', '--verbose',
        help="Print verbose test output.",
        action="store_true", dest="do_verbose",
        default=False)

    parser_test.add_argument('--extra',
        help="Run extra tests (requires building with --extra).",
        action="store_true", dest="do_extra_test",
        default=False)

    # configure install subcommand
    parser_install = subparsers_main.add_parser('install', add_help=False, help='Install Shadow.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser_install.set_defaults(func=install,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser_install.add_argument('-h', '--help', action='help', default=argparse.SUPPRESS, help=HELP_STR)

    # provide help by default
    if len(sys.argv) == 1:
        parser_main.print_help(sys.stderr)
        sys.exit(1)

    # parse arguments after '--' separately
    argv = sys.argv[1:]
    remaining = []
    if '--' in argv:
        index = argv.index('--')
        (argv, remaining) = (argv[:index], argv[index+1:])

    # get arguments, accessible with args.value
    args = parser_main.parse_args(argv)
    # run chosen command
    r = args.func(args, remaining)

    logging.debug("Setup is returning code '{0}'".format(r))
    sys.exit(r)

def has_bear():
    # Use the shell to see if bear exists. Capture output to prevent it being
    # printed to the console.
    return subprocess.run(
        "bear --version",
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE).returncode == 0

def build(args, remaining):
    # get absolute paths
    if args.prefix is not None: args.prefix = getfullpath(args.prefix)

    filepath=getfullpath(__file__)
    rootdir=filepath[:filepath.rfind("/")]
    builddir=getfullpath(BUILD_PREFIX)
    installdir=getfullpath(args.prefix)

    # workaround for elf-loader extract-system-config.py
    os.putenv("LANGUAGE", "C")

    # clear cmake cache
    if args.do_force_rebuild and os.path.exists(builddir): shutil.rmtree(builddir)

    # create directories
    if not os.path.exists(builddir): os.makedirs(builddir)
    if not os.path.exists(installdir): os.makedirs(installdir)

    # build up args string for the cmake command
    cmake_cmd = "cmake " + rootdir + " -DCMAKE_INSTALL_PREFIX=" + installdir

    # other cmake options
    cmake_cmd += " -DCMAKE_BUILD_TYPE=" + ("Debug" if args.do_debug else "Release")
    if args.do_verbose: os.putenv("VERBOSE", "1")
    if args.do_test: cmake_cmd += " -DSHADOW_TEST=ON"
    if args.do_werror: cmake_cmd += " -DSHADOW_WERROR=ON"
    if args.do_use_perf_timers: cmake_cmd += " -DSHADOW_USE_PERF_TIMERS=ON"

    if args.do_coverage:
        if not args.do_debug:
            logging.warning("You usually want to enable --debug for coverage"
                    + " builds")
        cmake_cmd += " -DSHADOW_COVERAGE=ON"

    # we will run from build directory
    calledDirectory = os.getcwd()

    # add extra search directories as absolution paths
    make_paths_absolute(args.search_prefix)

    # make sure we can access them from cmake
    cmake_cmd += " -DCMAKE_PREFIX_PATH=" + ';'.join(args.search_prefix)

    # look for the clang/clang++ compilers
 #   clangccpath = which("clang")
 #   if clangccpath is None:
 #       logging.error("can't find 'clang' compiler in your PATH! Is it installed?")
 #   clangcxxpath = which("clang++")
 #   if clangcxxpath is None:
 #       logging.error("can't find 'clang++' compiler in your PATH! Is it installed?")
 #   if clangccpath is None or clangcxxpath is None: return -1

    # set clang/llvm as compiler
 #   os.putenv("CC", clangccpath)
 #   os.putenv("CXX", clangcxxpath)
    #cmake_cmd += " -D_CMAKE_TOOLCHAIN_PREFIX=llvm-"

    # call cmake to configure the make process, wait for completion
    logging.info("running \'{0}\' from \'{1}\'".format(cmake_cmd, builddir))
    retcode = subprocess.call(cmake_cmd.strip().split(), cwd=builddir)
    logging.debug("cmake returned " + str(retcode))
    if retcode != 0:
        logging.error(f"Non-zero return code {retcode} from cmake.")
        return retcode

    # call make, wait for it to finish
    make = ""
    if has_bear():
        make += "bear --append --"
    make += f" make -j{args.njobs}"

    logging.info("calling " + make)
    retcode = subprocess.call(shlex.split(make), cwd=builddir)
    logging.debug("make returned " + str(retcode))
    if retcode != 0:
        logging.error(f"Non-zero return code {retcode} from make.")
        return retcode

    if args.do_extra_test:
        make += " extra_tests"
        logging.info("calling " + make)
        retcode = subprocess.call(shlex.split(make), cwd=builddir)
        logging.debug("make returned " + str(retcode))
        if retcode != 0:
            logging.error(f"Non-zero return code {retcode} from make.")
            return retcode

    return 0

def test(args, remaining):
    testdir=getfullpath(BUILD_PREFIX)

    if not os.path.exists(testdir+'/CTestTestfile.cmake'):
        logging.error("please run './setup build --test' before testing!")
        return -1

    # go to build dir and install from makefile
    calledDirectory = os.getcwd()
    os.chdir(testdir)

    # test, wait for it to finish
    testcmd = "ctest -j{0} --timeout {1}".format(args.njobs, args.timeout)
    if args.do_verbose:
        testcmd += " --verbose"

    if args.rerun_failed:
        testcmd += " --rerun-failed"

    if args.testname_regex is not None:
        testcmd += " -R {}".format(args.testname_regex)

    if args.do_extra_test:
        testcmd += " -C extra"

    # pass the remaining arguments (the arguments after a '--') directly to ctest
    testcmd = testcmd.strip().split() + remaining

    logging.info("calling '{}'".format(' '.join(testcmd)))
    retcode = subprocess.call(testcmd)
    logging.info(str(testcmd) + " returned " + str(retcode))

    # go back to where we came from
    os.chdir(calledDirectory)
    return retcode

def install(args, remaining):
    builddir=getfullpath(BUILD_PREFIX)
    if not os.path.exists(builddir):
        logging.error("please build before installing!")
        return -1

    # go to build dir and install from makefile
    calledDirectory = os.getcwd()
    os.chdir(builddir)

    # call make install, wait for it to finish
    makeCommand = "make install"

    logging.info("calling \'"+makeCommand+"\'")
    retcode = subprocess.call(makeCommand.strip().split())
    logging.debug("make install returned " + str(retcode))
    if retcode == 0: logging.info("now run \'shadow\' from \'PREFIX/bin\' (check your PATH!)")

    # go back to where we came from
    os.chdir(calledDirectory)
    return retcode

def getfullpath(path):
    return os.path.abspath(os.path.expanduser(path))

def make_paths_absolute(list):
    for i in range(len(list)): list[i] = getfullpath(list[i])

## helper - test if program is in path
def which(program):
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)
    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return None

if __name__ == '__main__':
    main()
