
#include "include/generators/city_gen.hpp"
#include <fmt/core.h>
#include <optional>

City CityGenerator::get() {
  fmt::print("Generating city for {}\n", this->rm->seed());

  while (this->gen->items.size() < 300) {
    auto fam_count = Random::get<int>(5, 10);
    auto single_count = Random::get<int>(5, 10);
    for (auto n = 0; n < fam_count; n++) {
      auto a = this->gen->get(PersonSpec{
          .age = intSpec{std::pair{80, 90}},
      });
      a->local = true;
      this->gen->property(a->id, "wealth", Random::get<int>(-1, 4));
      this->gen->property(a->id, "reputation", Random::get<int>(-1, 4));
      this->proliferate(a);
    }
    for (auto n = 0; n < single_count; n++) {
      auto a = this->gen->get(PersonSpec{
          .age = intSpec{std::pair{18, 90}},
      });
      this->gen->property(a->id, "wealth", Random::get<int>(-1, 4));
      this->gen->property(a->id, "reputation", Random::get<int>(-1, 4));
    }
  }
  this->populateOrgs();
  this->recalcWealth();
  this->recalcReputation();
  this->populateHouseholds();

  std::vector<std::shared_ptr<Group>> groups;

  std::ranges::copy(this->grpManager->items, std::back_inserter(groups));
  std::cerr << "DEBUGPRINT[4]: city_gen.cpp:36: this->grpManager->items.size()="
            << this->grpManager->items.size() << std::endl;
  std::cerr << "DEBUGPRINT[3]: city_gen.cpp:36: groups=" << groups.size()
            << std::endl;
  Random::shuffle(groups);
  for (auto g : groups) {
    std::cerr << "DEBUGPRINT[2]: city_gen.cpp:38: g=" << g->name << std::endl;
    if (g->group_type != "" && !g->roles.empty()) {
      auto gt = this->grpManager->grpTypes[std::string(g->group_type)];
      auto l_type = gt.location_type;
      auto l_multi = gt.location_multi;
      if (g->group_type == "household") {
        auto wealth = -999;
        for (auto m : this->grpManager->getMembers(g->id)) {
          auto _w = this->gen->property(m, "wealth");
          auto w = _w ? std::get<float>(*_w) : 0;
          if (w > wealth)
            wealth = w;
        }
        l_type = "residential";
        if (wealth > 2) {
          l_multi = true;
        }
      }
      if (l_type == "none")
        continue;
      if (!gt.location_per_role) {
        auto l = this->locationGen->get(l_type, l_multi);
        g->location = l->id;
        l->name = g->name;
      } else {
        auto roles = this->grpManager->roles->items |
                     std::views::filter([&](std::shared_ptr<GroupRole> r) {
                       return r->group_id == g->id;
                     }) |
                     to_vector();
        for (auto role : roles) {
          auto l = this->locationGen->get(l_type, l_multi);
          g->location = l->id;
          l->name = fmt::format(fmt::runtime("{}'s {}"), std::string(g->name), std::string(role->name));
        }
      }
    }
  }

  fmt::print("Making friends\n");
  for (auto p : this->gen->items) {
    if (p->age < 5)
      continue;
    auto c =
        intSpec{std::map<int, float>{{0, 0.1}, {1, 0.5}, {2, 0.3}, {3, 0.1}}}
            .get();
    for (auto i = 0; i < c; i++) {
      auto f = this->gen->find(this->gen->getSpec("friend", p));
      if (f) {
        this->relManager->add("friend", p->id, (*f)->id);
        auto _br = this->gen->property((*f)->id, "reputation");
        auto br = _br ? std::get<float>(*_br) : 0;
        auto r = br * 0.1;
        if (br < 0)
          r = br * 0.25;
        this->gen->propertyMod(p->id, "reputation", r);
        auto _ar = this->gen->property(p->id, "reputation");
        auto ar = _ar ? std::get<float>(*_ar) : 0;
        r = ar * 0.1;
        if (ar < 0)
          r = ar * 0.25;
        this->gen->propertyMod((*f)->id, "reputation", r);
      }
    }
  }

  fmt::print("Making love\n");
  std::function<void(std::shared_ptr<Person>)> _makeLove =
      [&](std::shared_ptr<Person> p) {
        auto rel = this->makeLove(p);
        if (!rel)
          return;
        auto l = this->gen->get((*rel)->target);
        if (l->dead) {
          (*rel)->active = false;
          if (Random::get<bool>(0.75)) {
            _makeLove(p);
          }
        }
      };
  for (auto p : this->gen->items) {
    _makeLove(p);
  }

  fmt::print("Making affairs\n");
  for (auto p : this->gen->items) {
    this->makeAffair(p);
  }

  fmt::print("Making classmates\n");
  std::vector<intSpec> ages = {
      intSpec{std::pair<int, int>{18, 25}},
      intSpec{std::pair<int, int>{26, 35}},
      intSpec{std::pair<int, int>{36, 45}},
      intSpec{std::pair<int, int>{46, 55}},
      intSpec{std::pair<int, int>{56, 65}},
      intSpec{std::pair<int, int>{66, 100}},
  };
  for (auto spec : ages) {
    auto persons = this->gen->findAll(PersonSpec{
                       .age = spec,
                       .local = boolSpec{true},
                   }) |
                   std::views::transform([](auto p) { return p->id; }) |
                   to_vector();
    this->relManager->add("classmate", persons);
  }

  fmt::print("Making mystery\n");
  genMystery();

  // add criminals, homeless
  auto a = this->gen->get(PersonSpec{
      .age = intSpec{std::pair{30, 50}},
      .dead = boolSpec{false},
  });
  auto fbi = this->grpManager->add("FBI", "FBI", "special agent", {a->id});
  this->gen->property(a->id, "weapon", "gun");

  auto city = City{
      .seed = rm->seed(),
      .name = "New Tokio",
  };

  // std::copy(gen->items.begin(), gen->items.end(),
  // std::back_inserter(city.persons));
  fmt::print("Making city for rust\n");
  for (auto p : gen->items) {
    city.persons.push_back(*p);
  }
  for (auto p : relManager->items) {
    city.relations.push_back(*p);
  }
  for (auto p : grpManager->items) {
    city.groups.push_back(*p);
  }
  for (auto p : grpManager->roles->items) {
    city.roles.push_back(*p);
  }
  for (auto p : entityManager->items) {
    city.entities.push_back(*p);
  }
  for (auto p : locationGen->items) {
    city.geo.locations.push_back(*p);
  }
  for (auto p : locationGen->streets->items) {
    city.geo.streets.push_back(*p);
  }
  for (auto p : locationGen->buildings->items) {
    city.geo.buildings.push_back(*p);
  }
  fmt::print("City is ready\n");

  return city;
}

void CityGenerator::populateHouseholds() {
  fmt::print("Households population\n");
  std::vector<int> processed;
  std::vector<std::shared_ptr<Group>> households;
  for (auto p : this->gen->items) {
    if (std::find(processed.begin(), processed.end(), p->id) != processed.end())
      continue;

    std::vector<int> hh;
    hh.push_back(p->id);

    auto _spouse = this->relManager->getSymmetric("marriage", p->id, true);
    if (!_spouse.empty())
      hh.push_back(_spouse[0]);

    auto children =
        this->relManager->getTargets("parent", p->id) |
        std::views::filter([&](int pid) {
          return this->gen->get(pid)->age < 18 &&
                 this->gen->get(pid)->name.last_name == p->name.last_name;
        });
    std::copy(children.begin(), children.end(), std::back_inserter(hh));

    auto hh_name = fmt::format(fmt::runtime("{}s' household"),
                               std::string(p->name.last_name));
    if (hh.size() == 1) {
      auto _hms =
          this->gen->findAll(this->gen->getSpec("housemate", p)) |
          std::views::filter([&](std::shared_ptr<Person> _p) {
            return p->id != _p->id &&
                   this->relManager->getSymmetric("marriage", _p->id).empty() &&
                   std::find(processed.begin(), processed.end(), _p->id) ==
                       processed.end();
          });
      std::vector<std::shared_ptr<Person>> hms;
      std::copy(_hms.begin(), _hms.end(), std::back_inserter(hms));

      if (!hms.empty()) {
        auto hm = hms.at(Random::get<int>(0, hms.size() - 1));
        hh.push_back(hm->id);
        hh_name = fmt::format(
            fmt::runtime("{} & {} household"), std::string(p->name.last_name),
            std::string(this->gen->get(hh[1])->name.last_name));
      }
    }

    auto g = this->grpManager->add(hh_name, "household", "member",
                                   hh | std::views::filter([&](int pid) {
                                     return !this->gen->get(pid)->dead;
                                   }) | to_vector());
    std::copy(hh.begin(), hh.end(), std::back_inserter(processed));
    households.push_back(g);
  }
}

void CityGenerator::populateOrgs() {
  fmt::print("Orgs population\n");
  auto church = this->grpManager->populate("church");
  auto members = this->grpManager->getMembers(church->id);
  this->relManager->add("parishioner", members);
  for (auto m : members) {
    this->gen->get(m)->tags.push_back(Tag{"Christian", "neutral"});
  }

  for (auto n = 0; n < iS(1, 3).get(); n++) {
    auto hobby = this->rm->get<std::string>("hobby", true);
    auto club = this->grpManager->populate("hobby");
    club->name = fmt::format(fmt::runtime("{} club"), hobby);
    auto members = this->grpManager->getMembers(club->id);
    this->relManager->add("clubmate", members);
    for (auto m : members) {
      this->gen->property(m, "hobby", hobby);
    }
  }

  this->grpManager->populate("goverment");
  this->grpManager->populate("police");
  this->grpManager->populate("post");
  this->grpManager->populate("services");
  this->grpManager->populate("clinic");

  this->grpManager->populate("school");
  for (auto n = 0; n < iS(2, 4).get(); n++) {
    this->grpManager->populate("cafe");
  }

  auto g = this->grpManager->populate("corp");
  g->name = this->rm->get<std::string>("corp", true);

  std::vector<std::pair<std::string, std::string>> stores = {
      {"Grocery store", "shopkeeper"},
      {"General store", "shopkeeper"},
      {"Hardware store", "shopkeeper"},
      {"Book store", "shopkeeper"},
      {"Pharmacy", "pharmacist"},
      {"Backery", "backer"},
      {"Gas station", "owner"},
      {"Barbershop", "barber"},
      {"Laundry", "owner"},
  };
  for (auto s : stores) {
    this->grpManager->reg(GroupType{
        .name = s.first,
        .titles = {s.first},
        .cat = "org",
        .callback =
            [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
              if (role->name == "co-owner")
                return;
              setOccupation(group, role);
            },
        .roleSpecs =
            {
                {
                    s.second,
                    S(30, 65, std::nullopt),
                },
                {
                    "worker",
                    S(18, 25, std::nullopt),
                },
            },
        .roleCounts =
            {
                {s.second, intSpec{1}},
                {"worker", iS(0, 1)},
            },
        .roleRels = {{s.second, {{"worker"}, {"boss", "coworker"}}}},
        .location_type = "commerical",
    });
    auto org = this->grpManager->populate(s.first);
    if (!org->roles.empty()) {
      auto r = this->grpManager->roles->get(org->roles[0]);
      if (!r->members.empty()) {
        auto o = this->gen->get(r->members[0]);
        auto _spouse = this->relManager->getSymmetric("marriage", o->id, true);
        if (!_spouse.empty()) {
          auto p = this->gen->get(_spouse[0]);
          if (p->occupation.title == "") {
            this->grpManager->add(org->id, "co-owner", {p->id});
          }
        }
      }
    }
  }

  g = this->grpManager->populate("salon");
  g->name = this->rm->get<std::string>("salon", true);
  this->grpManager->populate("construction");
  g = this->grpManager->populate("clinic");
  g->name = this->rm->get<std::string>("clinic", true);
  for (auto n = 0; n < iS(1, 2).get(); n++) {
    auto g = this->grpManager->populate("manufacturer");
    g->name = this->rm->get<std::string>("factory", true);
  }
  this->grpManager->populate("singles");

  fmt::print("Orgs population finished\n");
}

std::optional<std::shared_ptr<Relation>>
CityGenerator::makeAffair(std::shared_ptr<Person> p) {
  auto _spouse = this->relManager->getSymmetric("marriage", p->id, true);
  if (p->dead || p->age < 18 || p->age > 70 ||
      (!_spouse.empty() && Random::get<bool>(0.65)) ||
      (_spouse.empty() && Random::get<bool>(0.3))) {
    return std::nullopt;
  }
  auto friends = this->relManager->getSymmetric("friend", p->id, true) |
                 std::views::filter([&](int fi) {
                   auto f = this->gen->get(fi);
                   return f->male != p->male;
                 });
  auto coworkers = this->relManager->getSymmetric("coworker", p->id, true) |
                   std::views::filter([&](int fi) {
                     auto f = this->gen->get(fi);
                     return f->male != p->male;
                   });
  auto love_interests =
      this->relManager->getTargets("love", p->id, false, true);
  auto love_sources = this->relManager->getSources("love", p->id, false, true);

  std::vector<int> candidates;
  if ((love_sources.size() + love_interests.size() == 0)) {
    if (Random::get<bool>(0.5)) {
      std::ranges::copy(friends, std::back_inserter(candidates));
    } else if (Random::get<bool>(0.5)) {
      std::ranges::copy(coworkers, std::back_inserter(candidates));
    }
  }
  std::ranges::copy(love_interests, std::back_inserter(candidates));
  std::ranges::copy(love_sources, std::back_inserter(candidates));
  if (!_spouse.empty()) {
    auto s_friends =
        this->relManager->getSymmetric("friend", _spouse[0], true) |
        std::views::filter([&](int fi) {
          auto f = this->gen->get(fi);
          return f->male != p->male;
        });
    std::ranges::copy(s_friends, std::back_inserter(candidates));
  }
  candidates =
      candidates | std::views::filter([&](int i) {
        auto c = this->gen->get(i);
        return c->age >= 18 && std::ranges::find(_spouse, i) == _spouse.end() &&
               this->relManager->getRelations("affair", i, true).size() < 2;
      }) |
      to_vector();
  if (candidates.empty())
    return std::nullopt;
  auto lover = candidates.at(Random::get<int>(0, candidates.size() - 1));
  auto l_spouse = this->relManager->getSymmetric("marriage", lover, true);

  if (!_spouse.empty() || !l_spouse.empty()) {
    this->gen->propertyMod(p->id, "reputation", -1);
    this->gen->propertyMod(lover, "reputation", -1);
  }
  if (!_spouse.empty() && std::ranges::find_if(p->tags, [&](Tag t) {
                            return t.name == "Adulterer";
                          }) == p->tags.end()) {
    p->tags.push_back(Tag{"Adulterer", "negative"});
  }
  auto l = this->gen->get(lover);
  if (!l_spouse.empty() && std::ranges::find_if(l->tags, [&](Tag t) {
                             return t.name == "Adulterer";
                           }) == p->tags.end()) {
    l->tags.push_back(Tag{"Adulterer", "negative"});
  }

  auto rel = this->relManager->add("affair", p->id, lover);
  if (!_spouse.empty() && Random::get<bool>(0.25)) {
    this->gen->get(_spouse[0])
        ->knowledge.push_back(this->entityManager->to(rel));
  }
  if (!l_spouse.empty() && Random::get<bool>(0.25)) {
    this->gen->get(l_spouse[0])
        ->knowledge.push_back(this->entityManager->to(rel));
  }
  return rel;
}

std::optional<std::shared_ptr<Relation>>
CityGenerator::makeLove(std::shared_ptr<Person> p) {
  if (p->age < 18 || Random::get<bool>(0.15))
    return std::nullopt;

  auto _rels = this->relManager->getRelations("love", p->id) |
               std::views::filter([&](std::shared_ptr<Relation> r) {
                 return r->source == p->id;
               });
  if (!std::ranges::empty(_rels))
    return std::nullopt;

  auto _friend = this->relManager->getSymmetric("friend", p->id) |
                 std::views::filter([&](int fi) {
                   auto f = this->gen->get(fi);
                   return f->male != p->male;
                 }) |
                 to_vector();
  auto coworkers = this->relManager->getSymmetric("coworker", p->id, true) |
                   std::views::filter([&](int fi) {
                     auto f = this->gen->get(fi);
                     return f->male != p->male;
                   }) |
                   to_vector();
  auto findLove = [&](std::shared_ptr<Person> p)
      -> std::optional<std::shared_ptr<Relation>> {
    std::optional<int> f = std::nullopt;
    if (!_friend.empty() && Random::get<bool>(0.3)) {
      f = _friend.at(Random::get<int>(0, _friend.size() - 1));
    } else if (!coworkers.empty() && Random::get<bool>(0.3)) {
      f = coworkers.at(Random::get<int>(0, coworkers.size() - 1));
    } else {
      auto _f = this->gen->find(this->gen->getSpec("love", p));
      if (_f)
        f = (*_f)->id;
    }
    if (f) {
      return this->relManager->add("love", p->id, *f);
    }
    return std::nullopt;
  };

  auto _love = this->relManager->getTargets("love", p->id);
  auto _spouse = this->relManager->getSymmetric("marriage", p->id);

  std::optional<std::shared_ptr<Relation>> loveRel = std::nullopt;

  if (_love.size() > 0) {
    for (auto love : _love) {
      if (love == _spouse[0]) {
        if (this->gen->boolByAge(p->id, {
                                            {{18, 30}, 0.6},
                                            {{31, 45}, 0.4},
                                            {{46, 65}, 0.5},
                                            {{66, 100}, 0.8},
                                        })) {
          return this->relManager->add("love", p->id, love);
        }
      } else if (std::find(_friend.begin(), _friend.end(), love) !=
                 _friend.end()) {
        if (Random::get<bool>(0.75)) {
          return this->relManager->add("love", p->id, love);
        }
      } else if (Random::get<bool>(0.75)) {
        return this->relManager->add("love", p->id, love);
      }
    }
    return findLove(p);
  } else if (_spouse.size() > 0 && Random::get<bool>(0.5)) {
    auto spouse_id = _spouse[0];
    return this->relManager->add("love", p->id, spouse_id);
  } else {
    return findLove(p);
  }
}

std::shared_ptr<Person>
CityGenerator::getSpouseForPerson(std::shared_ptr<Person> a,
                                  bool ensure_unmarried) {
  auto spec = this->gen->getSpec("spouse", a);
  auto created = false;
  std::shared_ptr<Person> b;
  if (ensure_unmarried) {
    auto bs = this->gen->findAll(spec);
    // fmt::print(fmt::runtime("found unfiltered: {}\n"), bs.size());
    if (bs.size() == 0) {
      b = this->gen->get(spec);
      while (b->age < 18) {
        b = this->gen->get(spec);
      }
      created = true;
    } else {
      bs = bs | std::views::filter([&](std::shared_ptr<Person> p) {
             auto married =
                 this->relManager->getRelations("marriage", p->id, true)
                     .size() != 0;
             auto was_married_count =
                 this->relManager->getRelations("marriage", p->id).size();
             auto is_relatives = p->name.last_name == a->name.last_name ||
                                 p->name.maid_last_name == a->name.last_name ||
                                 p->name.last_name == a->name.maid_last_name;
             return !married && !is_relatives && was_married_count < 2;
           }) |
           to_vector();
      if (bs.size() == 0) {
        b = this->gen->get(spec);
        while (b->age < 18) {
          b = this->gen->get(spec);
        }
        created = true;
      } else {
        b = bs.at(Random::get<int>(0, bs.size() - 1));
        // fmt::print(fmt::runtime("got: {} - {} for {}\n"), *b,
        //            this->relManager->getRelations("marriage",
        //            b->id).size(), *a);
      }
    }
  } else {
    auto _b = this->gen->find(spec);
    if (!_b) {
      b = this->gen->get(spec);
      created = true;
    } else {
      b = *_b;
    }
  }
  if (created) {
    this->gen->property(b->id, "wealth", Random::get<int>(-1, 4));
    this->gen->property(b->id, "reputation", Random::get<int>(-1, 4));
  }
  return b;
}

std::optional<std::vector<int>>
CityGenerator::makeNuclearFamily(std::shared_ptr<Person> a) {
  std::vector<int> family;
  family.push_back(a->id);
  auto married =
      this->relManager->getRelations("marriage", a->id, true).size() != 0;

  auto p_single = 0.0;
  if (a->age <= 25)
    p_single = 0.60;
  if (a->age > 25 && a->age <= 50)
    p_single = 0.30;
  if (a->age > 50)
    p_single = 0.10;

  if (a->age < 18 || married || Random::get<bool>(p_single)) {
    return std::nullopt;
  }

  auto not_married = Random::get<bool>(0.1);
  auto spec_name = "spouse";
  auto b = this->getSpouseForPerson(a, !not_married);
  if (!not_married) {
    auto m = this->relManager->add("marriage", a->id, b->id);
    if (b->dead) {
      m->active = false;
      this->relManager->add("widow", a->id, b->id);
      not_married = true;
    }
    if (m->active && Random::get<bool>(0.3)) {
      m->active = false;
      this->relManager->add("divorce", a->id, b->id);
      not_married = true;
    }
  } else {
    fmt::print(fmt::runtime("{} not married with {}\n"), *a, *b);
    auto m = this->relManager->add("affair", a->id, b->id);
    m->active = false;
  }

  auto cc =
      intSpec{std::map<int, float>{{1, 0.5}, {2, 0.3}, {3, 0.15}, {4, 0.05}}}
          .get();
  auto children = this->relManager->getTargets("parent", a->id);
  cc -= children.size();
  if (cc < 0)
    cc = 0;
  if (!not_married) {
    cc = 1;
  }

  std::string eyes_g_a = this->gen->propertyStr(a->id, "eyes.color");
  std::string eyes_g_b = this->gen->propertyStr(b->id, "eyes.color");

  std::string skin_c_a = this->gen->propertyStr(a->id, "skin.color");
  std::string skin_c_b = this->gen->propertyStr(b->id, "skin.color");

  std::string hair_c_a = this->gen->propertyStr(a->id, "hair.color");
  std::string hair_c_b = this->gen->propertyStr(b->id, "hair.color");

  std::map<int, Dates> bds;
  children.clear();
  // std::vector<int> children;
  for (auto n = 0; n < cc; n++) {
    auto c = this->gen->get(this->gen->getSpec("child", a, b));
    c->local = true;

    auto both = 0;
    if (c->age >= 18) {
      for (auto parent : std::vector<std::shared_ptr<Person>>{a, b}) {
        for (auto tag : parent->tags | std::views::filter([](Tag t) {
                          return std::string(t.cat) == "trait";
                        })) {
          auto trait = (*std::ranges::find_if(this->traits, [&](TraitSpec t) {
            return t.tag.name == tag.name;
          }));
          if (trait.onInherit && trait.spec.match(c)) {
            if (trait.onInherit(c)) {
              c->tags.push_back(tag);

              this->gen->propertyMod(c->id, "reputation", trait.reputation);
              this->gen->propertyMod(c->id, "wealth", trait.wealth);
              fmt::print(fmt::runtime("{} got {} from {}\n"), *c,
                         std::string(tag.name), *parent);
            }
          }
          both++;
        }
      }
    }
    if (both > 1) {
      addRandomTrait(c);
    }

    if (eyes_g_a == eyes_g_b) {
      this->gen->property(c->id, "eyes.color", eyes_g_a);
    } else {
      this->gen->property(c->id, "eyes.color",
                          Random::get<bool>(0.5) ? eyes_g_a : eyes_g_b);
    }

    if (skin_c_a == skin_c_b) {
      this->gen->property(c->id, "skin.color", skin_c_a);
    } else {
      this->gen->property(c->id, "skin.color",
                          Random::get<bool>(0.5) ? skin_c_a : skin_c_b);
    }

    if (hair_c_a == hair_c_b) {
      this->gen->property(c->id, "hair.color", hair_c_a);
    } else {
      this->gen->property(c->id, "hair.color",
                          Random::get<bool>(0.5) ? hair_c_a : hair_c_b);
    }

    if (bds.find(c->birth.year) != bds.end()) {
      c->birth = bds[c->birth.year];
    } else {
      bds[c->birth.year] = c->birth;
    }

    auto ar = this->relManager->add("parent", a->id, c->id);
    auto br = this->relManager->add("parent", b->id, c->id);

    auto r = ar;
    if (b->male) {
      r = br;
    }
    if (Random::get<bool>(0.15)) {
      c->knowledge.push_back(this->entityManager->to(r));
    }
    if (Random::get<bool>(0.15)) {
      if (b->male) {
        b->knowledge.push_back(this->entityManager->to(r));
      } else {
        a->knowledge.push_back(this->entityManager->to(r));
      }
    }

    // L.info("c: {}", *c);
    children.push_back(c->id);
  }
  this->relManager->add("sibling", children);
  family = children;
  family.push_back(a->id);
  family.push_back(b->id);
  // this->relManager->add("family", family);
  //
  if (not_married && Random::get<bool>(0.75)) {
    this->makeNuclearFamily(a);
  }
  return family;
}

std::optional<std::vector<int>>
CityGenerator::proliferate(std::shared_ptr<Person> a) {
  std::vector<int> out;
  out.push_back(a->id);
  auto ret = this->makeNuclearFamily(a);
  if (ret) {
    auto children = this->relManager->getTargets("parent", a->id);
    // fmt::print("children: {}\n", fmt::join(children, ", "));
    for (auto child : children) {
      this->proliferate(gen->get(child));
    }
  }
  return out;
}

void CityGenerator::recalcReputation() {
  auto persons = this->gen->items;
  std::ranges::sort(
      persons, [](auto const &a, auto const &b) { return a->age > b->age; });
  for (auto p : persons) {
    auto _spouse = this->relManager->getSymmetric("marriage", p->id, true);
    auto _ar = this->gen->property(p->id, "reputation");
    auto ar = _ar ? std::get<float>(*_ar) : 0;
    if (!_spouse.empty()) {

      auto _br = this->gen->property(_spouse[0], "reputation");
      auto br = _br ? std::get<float>(*_br) : 0;

      auto r = ar + br / 2.0f;
      this->gen->property(p->id, "reputation", r);
      this->gen->property(_spouse[0], "reputation", r);

      for (auto c : this->relManager->getTargets("parent", p->id)) {
        this->gen->propertyMod(c, "reputation", r * 0.1);
      }
    } else {

      for (auto c : this->relManager->getTargets("parent", p->id)) {
        this->gen->propertyMod(c, "reputation", ar * 0.1);
      }
    }
  }
}

void CityGenerator::recalcWealth() {
  auto persons = this->gen->items;
  std::ranges::sort(
      persons, [](auto const &a, auto const &b) { return a->age > b->age; });
  for (auto p : persons) {
    auto _spouse = this->relManager->getSymmetric("marriage", p->id, true);
    auto _ar = this->gen->property(p->id, "wealth");
    auto ar = _ar ? std::get<float>(*_ar) : 0;
    if (!_spouse.empty()) {

      auto _br = this->gen->property(_spouse[0], "wealth");
      auto br = _br ? std::get<float>(*_br) : 0;

      auto r = std::max(ar, br);
      this->gen->property(p->id, "wealth", r);
      this->gen->property(_spouse[0], "wealth", r);

      auto ch = this->relManager->getTargets("parent", p->id);
      r -= ch.size() * 0.25;
      for (auto _c : ch) {

        if (this->gen->get(_c)->age < 21) {
          this->gen->property(_c, "wealth", r);
        } else {
          this->gen->propertyMod(_c, "wealth", r * 0.1);
        }
      }
    } else {

      auto ch = this->relManager->getTargets("parent", p->id);
      ar -= ch.size() * 0.25;
      for (auto _c : ch) {

        if (this->gen->get(_c)->age < 21) {
          this->gen->property(_c, "wealth", ar);
        } else {
          this->gen->propertyMod(_c, "wealth", ar * 0.1);
        }
      }
    }
  }
}

void CityGenerator::genMystery() {
  std::shared_ptr<Person> p;
  if (std::ranges::find(this->assigned_traits, "Murdered") ==
      this->assigned_traits.end()) {
    p = *this->gen->find(PersonSpec{
        .age = iS(18, 70),
        .dead = boolSpec{false},
        .occupation = boolSpec{true},
    });
    p->dead = true;
    p->death = Dates{
        .day = Random::get<int>(1, 28),
        .month = Random::get<int>(1, 12),
        .year = this->rm->currentYear,
    };
    p->tags.push_back(Tag{
        .name = "Murdered",
        .cat = "trait",
    });
  } else {
    // shouldnt be here
    p = *std::ranges::find_if(this->gen->items, [&](std::shared_ptr<Person> p) {
      return std::ranges::find_if(p->tags, [&](Tag t) {
               return t.name == "Murdered";
             }) != p->tags.end();
    });
  }

  std::vector<std::shared_ptr<Person>> suspects;
  auto _spouse = this->relManager->getSymmetric("marriage", p->id);
  for (auto s : _spouse) {
    auto sus = this->gen->get(s);
    suspects.push_back(sus);
  }

  auto children = this->relManager->getTargets("parent", p->id);
  for (auto s : children) {
    auto sus = this->gen->get(s);
    if (sus->age < 18)
      continue;
    suspects.push_back(sus);
  }

  std::vector<std::string> symmetric = {"friend", "affair", "rival", "sibling"};
  for (auto s : symmetric) {
    std::ranges::copy(
        this->relManager->getSymmetric(s, p->id) |
            std::views::transform([&](int id) { return this->gen->get(id); }) |
            std::views::filter(
                [&](std::shared_ptr<Person> p) { return p->age >= 18; }),
        std::back_inserter(suspects));
  }
  std::vector<std::string> sources = {"love"};
  for (auto s : sources) {
    std::ranges::copy(
        this->relManager->getSources(s, p->id) |
            std::views::transform([&](int id) { return this->gen->get(id); }) |
            std::views::filter(
                [&](std::shared_ptr<Person> p) { return p->age >= 18; }),
        std::back_inserter(suspects));
  }

  for (auto l : this->relManager->getSymmetric("affair", p->id)) {
    auto spouse = this->relManager->getSymmetric("marriage", l, true);
    std::ranges::copy(spouse | std::views::transform([&](int id) {
                        return this->gen->get(id);
                      }) | std::views::filter([&](std::shared_ptr<Person> p) {
                        return p->age >= 18;
                      }),
                      std::back_inserter(suspects));
    auto loves_lovers = this->relManager->getSources("love", l);
    std::ranges::copy(loves_lovers | std::views::transform([&](int id) {
                        return this->gen->get(id);
                      }) | std::views::filter([&](std::shared_ptr<Person> p) {
                        return p->age >= 18;
                      }),
                      std::back_inserter(suspects));
  }

  // TODO: lovers spouse, and who love lovers
  // TODO: who also love victims love
  // TODO: somebofy from work
  // TODO: who loves victim and their spouse (stalkers)
  // Random suspects, criminals, etc

  for (auto sus : suspects) {
    if (sus->dead)
      continue;
    this->relManager->add("suspect", p->id, sus->id);
  }
}
