#include "backward.hpp"

#define BACKWARD_HAS_DWARF 1
#define BACKWARD_HAS_DW 1

namespace backward {
backward::SignalHandling sh;
} // namespace backward

#include "facade.hpp"
#include "include/generators/city_gen.hpp"
#include <memory>

#include <extism.hpp>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace extism;

std::vector<uint8_t> read(const char *filename) {
  std::ifstream file(filename, std::ios::binary);
  return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
}

City init(rust::String root, int32_t seed) {
  auto wasm = read((std::string(root) + "/code.wasm").c_str());
  std::string tmp = "Testing";

  // A lambda can be used as a host function
  auto hello_world = [&tmp](CurrentPlugin plugin,
                            const std::vector<Val> &inputs,
                            std::vector<Val> &outputs, void *user_data) {
    std::cout << "Hello from C++" << std::endl;
    std::cout << (const char *)user_data << std::endl;
    std::cout << tmp << std::endl;
    outputs[0].v = inputs[0].v;
  };

  std::vector<Function> functions = {
      Function("hello_world", {ValType::I64}, {ValType::I64}, hello_world,
               (void *)"Hello again!",
               [](void *x) { std::cout << "Free user data" << std::endl; }),
  };

  Plugin plugin(wasm, true, functions);

  const char *input = "this is a test";
  ExtismSize length = strlen(input);

  extism::Buffer output = plugin.call("count_vowels", (uint8_t *)input, length);
  std::cout << (char *)output.data << std::endl;

  auto gen = std::make_shared<CityGenerator>(std::string(root), seed);
  return gen->get();
}
