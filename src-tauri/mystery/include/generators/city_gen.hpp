#pragma once
#include "../facade.hpp"

#include "../db_manager.hpp"
#include "../entity_manager.hpp"
#include "../group_manager.hpp"
#include "../randomizer.hpp"
#include "../relation_manager.hpp"
#include "../specs/person_spec.hpp"
#include "../utils.hpp"
#include "./date_gen.hpp"
#include "./location_gen.hpp"
#include "./name_gen.hpp"
#include "./person_gen.hpp"

#include <filesystem>
#include <iostream>
#include <memory>

class TraitSpec {
public:
  Tag tag;
  PersonSpec spec;
  float weight;
  float reputation = 0;
  float wealth = 0;
  std::function<bool(std::shared_ptr<Person>)> onInherit;
};

class CityGenerator {
  std::shared_ptr<RManager> rm;
  std::shared_ptr<DbManager> db;
  std::shared_ptr<RelationManager> relManager;
  std::shared_ptr<EntityManager> entityManager;
  std::shared_ptr<GroupManager> grpManager;
  std::shared_ptr<PersonGenerator> gen;
  std::shared_ptr<LocationGenerator> locationGen;
  std::vector<TraitSpec> traits;
  std::vector<std::string> assigned_traits;

  PersonSpec
  S(int a, int b,
    std::optional<std::variant<bool, float, std::map<bool, float>>> m) {
    auto spec = PersonSpec{
        .male = boolSpec{m},
        .age = intSpec{std::pair<int, int>{a, b}},
        .dead = boolSpec{false},
        .occupation = boolSpec{false},
    };
    return spec;
  }

  intSpec iS(int a, int b) { return intSpec{std::pair<int, int>{a, b}}; }

  void setOccupation(std::shared_ptr<Group> group,
                     std::shared_ptr<GroupRole> role) {
    auto t = std::string(role->name);
    t[0] = std::toupper(t[0]);
    for (auto id : role->members) {
      auto p = this->gen->get(id);
      p->occupation = Occupation{
          .title = t,
          .role_id = role->id,
          .group_id = group->id,
      };
    }
  };

  void init();
  void initRelations();
  void initOrgs();
  void initTraits();
  void addRandomTrait(std::shared_ptr<Person> p);
  void genMystery();

  std::optional<std::vector<int>> makeNuclearFamily(std::shared_ptr<Person> a);
  std::optional<std::vector<int>> proliferate(std::shared_ptr<Person> a);
  std::shared_ptr<Person> getSpouseForPerson(std::shared_ptr<Person> a,
                                             bool ensure_unmarried = true);
  std::optional<std::shared_ptr<Relation>> makeLove(std::shared_ptr<Person> a);
  std::optional<std::shared_ptr<Relation>>
  makeAffair(std::shared_ptr<Person> a);

  void populateOrgs();
  void populateHouseholds();
  void recalcReputation();
  void recalcWealth();

public:
  CityGenerator(std::string root, int seed) {
    auto db = std::make_shared<DbManager>();
    auto r = std::string(root) + "/resources/";
    this->rm = std::make_shared<RManager>(db);
    this->rm->seed(seed);
    this->rm->currentYear = 1983;
    for (const auto &entry : std::filesystem::directory_iterator(r)) {
      if (entry.path().extension() == ".json") {
        this->rm->reg(entry.path().stem(), entry.path());
      }
    }

    this->gen = this->rm->reg<PersonGenerator>("person");
    this->locationGen = std::make_shared<LocationGenerator>(this->rm);
    this->rm->reg<NameGenerator>("name");
    this->rm->reg<DateGenerator>("date");

    init();
  }
  City get();
};
