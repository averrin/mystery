use cxx_build::CFG;

fn main() {
    CFG.include_prefix = "";
    // println!("cargo:rustc-link-search=/usr/lib/libnoise.so");
    // println!("cargo:rustc-link-lib=sqlite3");
    // println!("cargo:rustc-link-lib=SQLiteCpp");
    // println!("cargo:rustc-flags=-lnoise");
    println!("cargo:rustc-link-lib=fmt");
    println!("cargo:rerun-if-changed=mystery/include/facade.hpp");
    println!("cargo:rerun-if-changed=mystery/include/randomizer.hpp");
    println!("cargo:rerun-if-changed=mystery/include/generator.hpp");
    println!("cargo:rerun-if-changed=mystery/include/city_gen.hpp");
    println!("cargo:rerun-if-changed=mystery/include/generators/person_gen.hpp");
    println!("cargo:rerun-if-changed=mystery/include/generators/name_gen.hpp");
    println!("cargo:rerun-if-changed=mystery/include/generators/date_gen.hpp");
    println!("cargo:rerun-if-changed=mystery/include/specs/common_spec.hpp");
    println!("cargo:rerun-if-changed=mystery/include/specs/person_spec.hpp");
    println!("cargo:rerun-if-changed=mystery/include/specs/names_spec.hpp");
    // println!("cargo:rerun-if-changed=mystery/include/libprint/include/libprint.hpp");
    println!("cargo:rerun-if-changed=mystery/main.cpp");
    cxx_build::bridges(["src/main.rs", "src/common.rs"])
    // cc::Build::new()
        .debug(true)
        .cpp(true)
        .files([
            "mystery/main.cpp",
            "mystery/city_gen.cpp",
            "mystery/city_gen_init.cpp",
            "mystery/generators/person_gen.cpp",
            "mystery/generators/name_gen.cpp",
            "mystery/generators/date_gen.cpp",
        ])
        .include("mystery/include")
        .include("mystery/include/fmt/include")
        .flag("-w")
        .flag("-std=c++2b")
        .flag("-fno-permissive")
        .compile("mystery");
  tauri_build::build()
}
