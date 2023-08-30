#pragma once
#include "../generator.hpp"
#include "../randomizer.hpp"
#include "../specs/dates_spec.hpp"
#include "../../../target/cxxbridge/src/common.rs.h"
#include <fmt/core.h>

class DateGenerator: public Generator<Dates, DatesSpec> {
public:
  DateGenerator(std::shared_ptr<RManager> _rm, std::shared_ptr<DbManager> _db) : Generator(_rm, _db) {}
  std::shared_ptr<Dates> get(DatesSpec);

  std::shared_ptr<Dates> get() {
    return Generator::get();
  }

  std::shared_ptr<Dates> get(int i) {
    return Container::get(i);
  }

  std::optional<std::shared_ptr<Dates>> find(DatesSpec);
  std::vector<std::shared_ptr<Dates>> findAll(DatesSpec);
};

template <> struct fmt::formatter<Dates> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }
  auto format(Dates date, format_context& ctx) const {
    return fmt::format_to(ctx.out(), fmt::runtime("{}.{}.{}"), date.day, date.month, date.year);
  };
};


