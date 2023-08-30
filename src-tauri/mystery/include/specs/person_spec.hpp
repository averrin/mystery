#pragma once
#include "dates_spec.hpp"
#include "names_spec.hpp"
#include <fmt/core.h>

struct PersonSpec {
  boolSpec male;
  intSpec age;

  NamesSpec name;
  DatesSpec birth;
  DatesSpec death;
  boolSpec dead = boolSpec{0.15f};
  boolSpec local;
  boolSpec occupation;

  //TODO add to match
  stringSpec occupation_title;
  stringSpec occupation_group;
  stringSpec occupation_role;
  stringSpec hasTag;

  // TODO: name match
  bool match(std::shared_ptr<Person> mv) {
    auto res = this->male.match(mv->male) && this->age.match(mv->age) &&
               this->dead.match(mv->dead) && this->local.match(mv->local) &&
               this->occupation.match(mv->occupation.title != "");
    return res;
  }
};

template <> struct fmt::formatter<PersonSpec> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  auto format(PersonSpec spec, format_context &ctx) const {
    return fmt::format_to(ctx.out(), "<male: {}; dead: {}, age: {}, name: {}>",
                          spec.male, spec.dead, spec.age, spec.name);
  };
};
