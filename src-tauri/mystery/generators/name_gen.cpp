#include "generators/name_gen.hpp"

std::shared_ptr<Names> NameGenerator::get(NamesSpec spec) {
  auto male = spec.male.get();
  auto name = std::make_shared<Names>();
  auto fn = "male_name";
  if (!male) {
    fn = "female_name";
  }

  auto from_spec = spec.first_name.get();
  name->first_name = from_spec != "" ? from_spec : this->rm->get<std::string>(fn);
  from_spec = spec.middle_name.get();
  if (from_spec != "" || Random::get<bool>(0.03)) {
    name->middle_name = from_spec != "" ? from_spec : this->rm->get<std::string>(fn);
  }
  from_spec = spec.last_name.get();
  name->last_name = from_spec != "" ? from_spec : this->rm->get<std::string>("last_name", true);

  return this->get(this->save(name));
}

std::optional<std::shared_ptr<Names>> NameGenerator::find(NamesSpec) {}
std::vector<std::shared_ptr<Names>> NameGenerator::findAll(NamesSpec) {}
