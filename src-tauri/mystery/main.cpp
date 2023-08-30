#include "backward.hpp"

#define BACKWARD_HAS_DWARF 1
#define BACKWARD_HAS_DW 1

namespace backward {
backward::SignalHandling sh;
} // namespace backward


#include "facade.hpp"
#include <memory>
#include "include/generators/city_gen.hpp"

City init(rust::String root, int32_t seed) {
  auto gen = std::make_shared<CityGenerator>(std::string(root), seed);
  return gen->get();
}
