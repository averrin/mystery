#pragma once
#include "common_specs.hpp"
#include "../../../target/cxxbridge/src/common.rs.h"

struct NamesSpec {
  boolSpec male;
  stringSpec first_name;
  stringSpec middle_name;
  stringSpec last_name;

  bool match(Names mv) {
    return this->first_name.match(std::string(mv.first_name))
    && this->middle_name.match(std::string(mv.middle_name))
    && this->last_name.match(std::string(mv.last_name));
  }
};

template <> struct fmt::formatter<NamesSpec> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }
  auto format(NamesSpec spec, format_context& ctx) const {
    return fmt::format_to(ctx.out(),
                          "<male: {}, {} {} {}>",
                          spec.male,
                          spec.first_name,
                          spec.middle_name,
                          spec.last_name
      );
  };
};
