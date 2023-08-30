#pragma once
#include "common_specs.hpp"
#include "../../../target/cxxbridge/src/common.rs.h"

struct DatesSpec {
  intSpec day = intSpec{std::pair<int, int>{1, 31}};
  intSpec month = intSpec{std::pair<int, int>{1, 12}};
  intSpec year;
  intSpec age;
  int currentYear;

  bool match(Dates mv) {
    if (this->year.match(mv.year)) {
      if (this->month.match(mv.month)) {
        return intSpec{std::pair<int, int>{1, this->day.get()}}.match(mv.day);
      }
      return intSpec{std::pair<int, int>{1, this->month.get()}}.match(mv.month);
    }
    return intSpec{std::pair<int, int>{1, this->year.get()}}.match(mv.year);
  }
};

template <> struct fmt::formatter<DatesSpec> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  auto format(DatesSpec spec, format_context &ctx) const {
    return fmt::format_to(ctx.out(), "<{}.{}.{} age:{}, cy:{}>", spec.day,
                          spec.month, spec.year, spec.age, spec.currentYear);
  };
};
