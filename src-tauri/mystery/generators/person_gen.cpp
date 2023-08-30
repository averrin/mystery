#include "generators/person_gen.hpp"
#include "generators/date_gen.hpp"
#include "generators/name_gen.hpp"

#include "utils.hpp"
#include <fmt/format.h>
#include <optional>

const int dead_after = 90;
const int default_max_age = 120;

std::shared_ptr<Person> PersonGenerator::get(PersonSpec spec) {
  // fmt::print("spec: {}\n", spec);
  auto ns = spec.name;
  ns.male.set(spec.male.get());
  auto name = *this->rm->getGen<NameGenerator>("name")->get(ns);
  auto dgen = this->rm->getGen<DateGenerator>("date");
  auto bs = spec.birth;
  auto age = spec.age.get_or(intSpec{std::pair{0, default_max_age}});
  bs.age.set(age);
  auto birth = *dgen->get(bs);

  auto person = std::make_shared<Person>(
      Person{.name = name,
             .birth = birth,
             .dead = age <= dead_after ? spec.dead.get() : true,
             .age = age,
             .male = ns.male.get()});

  if (spec.death.year.value || person->dead) {
    person->dead = true;
    if (!spec.death.year.value) {
      spec.death.year = intSpec{
          std::max(this->rm->currentYear -
                       Random::get<int>(0, spec.age.get_or(intSpec{80})),
                   person->birth.year)};
    }
    auto death = *dgen->get(spec.death);
    if (death.year == birth.year) {
      death.month = std::max(death.month, birth.month);
      death.day = std::max(death.month, birth.day);
    }
    person->age = death.year - birth.year;
    person->death = death;
  } else {
    person->age = rm->currentYear - birth.year;
  }

  auto id = this->save(person);
  for (auto cb : this->onCreate) {
    cb(person);
  }
  return this->get(id);
}

std::optional<std::shared_ptr<Person>> PersonGenerator::find(PersonSpec spec) {
  auto _persons =
      this->items | std::views::filter([&](std::shared_ptr<Person> p) {
        return spec.match(p);
      });
  std::vector<std::shared_ptr<Person>> persons;
  for (auto p : _persons) {
    persons.push_back(p);
  }
  if (persons.size() == 0)
    return std::nullopt;
  return persons.at(Random::get<int>(0, persons.size() - 1));
}
std::vector<std::shared_ptr<Person>> PersonGenerator::findAll(PersonSpec spec) {
  auto _persons =
      this->items | std::views::filter([&](std::shared_ptr<Person> p) {
        return spec.match(p);
      }); //' | to_vector();
  std::vector<std::shared_ptr<Person>> persons;
  for (auto p : _persons) {
    persons.push_back(p);
  }
  return persons;
}

std::optional<std::variant<float, std::string>>
PersonGenerator::property(int id, std::string name) {
  std::optional<std::variant<float, std::string>> value = std::nullopt;
  auto person = this->get(id);
  auto p = std::ranges::find_if(person->properties,
                                [&](Property p) { return p.name == name; });
  if (p != person->properties.end()) {
    if ((*p).s != "") {
      value = std::string((*p).s);
    } else {
      value = (*p).n;
    }
  }

  return value;
}
void PersonGenerator::property(int id, std::string name, float v) {
  auto person = this->get(id);
  auto p = std::ranges::find_if(person->properties,
                                [&](Property p) { return p.name == name; });

  // fmt::print(fmt::runtime("for {} set {}: "), id, name);
  if (p != person->properties.end()) {
    // fmt::print(fmt::runtime(" {} "), std::get<float>(*property(id, name)));
    auto i = std::distance(person->properties.begin(), p);
    person->properties[i].n = v;
  } else {
    person->properties.push_back(Property{
        .name = name,
        .n = v,
    });
  }
  // fmt::print(fmt::runtime("=> {}\n"), std::get<float>(*property(id, name)));
}
void PersonGenerator::property(int id, std::string name, std::string v) {
  auto person = this->get(id);
  auto p = std::ranges::find_if(person->properties,
                                [&](Property p) { return p.name == name; });

  if (p != person->properties.end()) {
    auto i = std::distance(person->properties.begin(), p);
    person->properties[i].s = v;
  } else {
    person->properties.push_back(Property{
        .name = name,
        .s = v,
    });
  }
}

void PersonGenerator::propertyMod(int id, std::string name, float v) {
  auto p = property(id, name);
  if (p) {
    property(id, name, std::get<float>(*p) + v);
  } else {
    property(id, name, v);
  }
}

template <class T> T PersonGenerator::property(int id, std::string name) {
  auto _p = property(id, name);
  if (_p) {
    return std::get<T>(*_p);
  } else {
    if (std::is_same<T, std::string>::value) return "";
    if (std::is_same<T, float>::value) return 0;
  }
}
