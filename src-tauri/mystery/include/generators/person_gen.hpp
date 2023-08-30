#pragma once
#include "../generator.hpp"
#include "../specs/person_spec.hpp"
#include "../../../target/cxxbridge/src/common.rs.h"
#include <fmt/core.h>

class PersonGenerator : public Generator<Person, PersonSpec> {
public:
  std::vector<std::function<void(std::shared_ptr<Person>)>> onCreate;
  std::shared_ptr<Container<Property>> properties;
  PersonGenerator(std::shared_ptr<RManager> _rm, std::shared_ptr<DbManager> _db)
      : Generator(_rm, _db) {
    properties = std::make_shared<Container<Property>>();
  }
  std::shared_ptr<Person> get(PersonSpec);
  std::shared_ptr<Person> get() { return Generator::get(); }
  std::shared_ptr<Person> get(int i) { return Container::get(i); }

  std::shared_ptr<Person> find_or_get(PersonSpec spec) {
    auto p = this->find(spec);
    if (p)
      return *p;
    return this->get(spec);
  }

  std::optional<std::shared_ptr<Person>> find(PersonSpec);
  std::vector<std::shared_ptr<Person>> findAll(PersonSpec);

  bool boolByAge(int id, std::map<std::pair<int, int>, float> v) {
    auto person = this->get(id);
    for (auto [pair, f] : v) {
      if (pair.first <= person->age && person->age <= pair.second) {
        return Random::get<bool>(f);
      }
    }
    return Random::get<bool>();
  }

  std::optional<std::variant<float, std::string>> property(int id,
                                                           std::string name);
  template <class T> T property(int id, std::string name);
  void property(int id, std::string name, float v);
  void propertyMod(int id, std::string name, float v);
  void property(int id, std::string name, std::string v);

  std::string propertyStr(int id, std::string name) {
    auto _p = property(id, name);
    if (_p) {
      return std::get<std::string>(*_p);
    } else {
      return "";
    }
  }
};

template <> struct fmt::formatter<Person> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  auto format(Person person, format_context &ctx) const {
    return fmt::format_to(ctx.out(), fmt::runtime("[{}] {} ({} - {})"),
                          person.id, person.name, person.birth, person.age);
  };
};
