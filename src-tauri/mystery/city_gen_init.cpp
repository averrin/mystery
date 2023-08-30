#include "include/generators/city_gen.hpp"
#include <fmt/core.h>
#include <optional>

void CityGenerator::init() {

  initTraits();

  this->gen->onCreate.push_back([&](std::shared_ptr<Person> p) {
    std::vector features = {
        "eyes.general",   "eyes.color", "skin.general", "skin.color",
        "face.structure", "face.nose",  "face.mouth",

        "hair.general",   "hair.color", "body.type",    "hands",
    };
    for (auto f : features) {
      this->gen->property(p->id, f, this->rm->get<std::string>(f));
    }
    if (p->male && p->age > 25 && Random::get<bool>(0.3)) {
      auto f = "face.hair";
      this->gen->property(p->id, f, this->rm->get<std::string>(f));
    }

    auto personality = this->rm->get("personality");
    this->gen->property(p->id, "personality.general",
                        personality["title"].template get<std::string>());
    std::vector<std::string> personalityKeys = {
        "psychology",     "selfDiscipline",    "lifestyle",
        "socialAttitude", "honesty",           "intelligence",
        "positivity",     "emotionalCapacity", "currentEmotionalState",
        "manners",        "assertiveness",     "rebelliousness",
        "activeness",     "confidence"};

    auto pt = personality["personalityTypes"];
    for (const std::string &key : personalityKeys) {
      this->gen->property(p->id, "personality." + key,
                          (*Random::get(pt[key])).template get<std::string>());
    }

    p->tags.push_back(Tag{
        .name = this->rm->get<std::string>("traits.positive"),
        .cat = "positive",
    });
    p->tags.push_back(Tag{
        .name = this->rm->get<std::string>("traits.neutral"),
        .cat = "neutral",
    });
    p->tags.push_back(Tag{
        .name = this->rm->get<std::string>("traits.negative"),
        .cat = "negative",
    });

    if (p->age > 18 && ((p->male && Random::get<bool>(0.4)) ||
                        (!p->male && Random::get<bool>(0.2)))) {
      addRandomTrait(p);
    }
  });

  initRelations();
  initOrgs();
}

void CityGenerator::addRandomTrait(std::shared_ptr<Person> p) {
  std::map<int, float> trait_weights;
  for (auto n = 0; n < this->traits.size(); n++) {
    trait_weights[n] = float(this->traits[n].weight);
  }
  auto trait_gen = intSpec{trait_weights};

  auto t_n = trait_gen.get();
  auto trait = this->traits[t_n];
  auto n = 0;
  while (!trait.spec.match(p)) {
    t_n = trait_gen.get();
    trait = this->traits[t_n];
    n++;
    if (n > 5)
      return;
  }
  p->tags.push_back(trait.tag);
  this->gen->propertyMod(p->id, "wealth", trait.wealth);
  this->gen->propertyMod(p->id, "reputation", trait.reputation);
  this->assigned_traits.push_back(std::string(trait.tag.name));
}

void CityGenerator::initTraits() {
  this->traits = std::vector<TraitSpec>{
      TraitSpec{
          .tag =
              Tag{
                  .name = "Alcoholism",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.4f,
          .reputation = -2.0f,
          .wealth = -2.0f,
          .onInherit =
              [](std::shared_ptr<Person> p) { return Random::get<bool>(0.4); }},

      TraitSpec{
          .tag =
              Tag{
                  .name = "Attempted suicide",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.15f,
      },

      TraitSpec{
          .tag =
              Tag{
                  .name = "Suicide",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
                  .dead = boolSpec{true},
              },
          .weight = 0.15f,
      },
      TraitSpec{
          .tag =
              Tag{
                  .name = "Bullying",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.3f,
          .reputation = -1.0f,
          .wealth = -0.0f,
      },

      TraitSpec{
          .tag =
              Tag{
                  .name = "Domestic violence",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.3f,
          .reputation = -1.0f,
          .wealth = -0.0f,
          .onInherit =
              [](std::shared_ptr<Person> p) { return Random::get<bool>(0.5); }},

      TraitSpec{
          .tag =
              Tag{
                  .name = "Drug addiction",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 60),
              },
          .weight = 0.2f,
          .reputation = -1.0f,
          .wealth = -2.0f,
      },

      TraitSpec{
          .tag =
              Tag{
                  .name = "Erotomania",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.1f,
      },

      TraitSpec{
          .tag =
              Tag{
                  .name = "Exhibitionism",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.1f,
          .reputation = -3.0f,
          .wealth = -0.0f,
          .onInherit =
              [](std::shared_ptr<Person> p) { return Random::get<bool>(0.1); }},

      TraitSpec{
          .tag =
              Tag{
                  .name = "Harassment",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.2f,
      },

      // TraitSpec{
      //     .tag =
      //         Tag{
      //             .name = "Killer",
      //             .cat = "trait",
      //         },
      //     .spec =
      //         PersonSpec{
      //             .age = iS(18, 70),
      //         },
      //     .weight = 0.1f,
      // },

      // TraitSpec{
      //     .tag =
      //         Tag{
      //             .name = "Serial Killer",
      //             .cat = "trait",
      //         },
      //     .spec =
      //         PersonSpec{
      //             .male = boolSpec{true},
      //             .age = iS(18, 70),
      //         },
      //     .weight = 0.05f,
      //     .onInherit =
      //         [](std::shared_ptr<Person> p) { return Random::get<bool>(0.2);
      //         }},

      TraitSpec{
          .tag =
              Tag{
                  .name = "Gambling",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.2f,
          .reputation = -1.0f,
          .wealth = -2.0f,
      },

      TraitSpec{
          .tag =
              Tag{
                  .name = "Kleptomania",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.1f,
          .onInherit =
              [](std::shared_ptr<Person> p) { return Random::get<bool>(0.1); }},

      TraitSpec{
          .tag =
              Tag{
                  .name = "Psychopathy",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.1f,
          .onInherit =
              [](std::shared_ptr<Person> p) { return Random::get<bool>(0.3); }},

      TraitSpec{
          .tag =
              Tag{
                  .name = "Pyromania",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.1f,
          .onInherit =
              [](std::shared_ptr<Person> p) { return Random::get<bool>(0.2); }},

      TraitSpec{
          .tag =
              Tag{
                  .name = "Shoplifting",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.4f,
      },

      TraitSpec{
          .tag =
              Tag{
                  .name = "Stalking",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.1f,
      },

      TraitSpec{
          .tag =
              Tag{
                  .name = "Voyeurism",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(18, 70),
              },
          .weight = 0.1f,
      },

      TraitSpec{
          .tag =
              Tag{
                  .name = "Workplace violence",
                  .cat = "trait",
              },
          .spec =
              PersonSpec{
                  .age = iS(20, 70),
              },
          .weight = 0.2f,
          .onInherit =
              [](std::shared_ptr<Person> p) { return Random::get<bool>(0.2); }},
  };
}

void CityGenerator::initRelations() {
  this->gen->addSpec(
      "spouse", PersonGenerator::specFunc{[](std::shared_ptr<Person> source) {
        auto r = intSpec{std::map<int, float>{{10, 0.9}, {20, 0.1}}}.get();
        return PersonSpec{
            .male = {!source->male},
            .age = intSpec{std::pair{std::max(source->age - r, 18),
                                     std::min(source->age + r, 90)}},
        };
      }});

  this->gen->addSpec(
      "love", PersonGenerator::specFunc{[](std::shared_ptr<Person> source) {
        auto r = intSpec{std::map<int, float>{{10, 0.9}, {20, 0.1}}}.get();
        return PersonSpec{
            .male = {!source->male},
            .age = intSpec{std::pair{std::max(source->age - r, 18),
                                     std::min(source->age + r, 90)}},
        };
      }});

  this->gen->addSpec(
      "friend", PersonGenerator::specFunc{[](std::shared_ptr<Person> source) {
        auto r = intSpec{std::map<int, float>{{10, 0.9}, {20, 0.1}}}.get();
        return PersonSpec{
            .age = intSpec{std::pair{std::max(source->age - r, 18),
                                     std::min(source->age + r, 90)}},
        };
      }});

  this->gen->addSpec(
      "housemate",
      PersonGenerator::specFunc{[](std::shared_ptr<Person> source) {
        auto r = intSpec{std::map<int, float>{{10, 0.9}, {20, 0.1}}}.get();
        return PersonSpec{
            .male = {source->male},
            .age = intSpec{std::pair{std::max(source->age - r, 18),
                                     std::min(source->age + r, 90)}},
        };
      }});

  this->gen->addSpec(
      "child", PersonGenerator::specFunc{[this](std::shared_ptr<Person> a,
                                                std::shared_ptr<Person> b) {
        auto father = a;
        auto mother = b;
        if (!father->male) {
          father = b;
          mother = a;
        }
        auto ma = intSpec{std::map<int, float>{{35, 0.9}, {50, 0.1}}}.get();
        auto m_age = std::min(father->age, mother->age);
        auto max_age = std::max(m_age - 18, 0);
        auto min_age = std::max(m_age - ma, 0);
        auto age = intSpec{std::pair{min_age, max_age}}.get();
        auto ns = NamesSpec{
            .last_name = stringSpec{std::string(mother->name.last_name)},
        };
        if (!mother->dead || this->rm->currentYear - age < mother->death.year) {
          if (!father->dead ||
              this->rm->currentYear - age < father->death.year) {
            return PersonSpec{
                .age = intSpec{age},
                .name = ns,
            };
          } else {
            return PersonSpec{
                .name = ns,
                .birth =
                    DatesSpec{
                        .month = intSpec{std::pair<int, int>{
                            1, std::min(12, father->death.month + 9)}},
                        .year = intSpec{father->death.year},
                    },
            };
          }
        } else {
          return PersonSpec{
              .name = ns,
              .birth =
                  DatesSpec{
                      .day = intSpec{mother->death.day},
                      .month = intSpec{mother->death.month},
                      .year = intSpec{mother->death.year},
                  },
          };
        }
      }});

  this->relManager = std::make_shared<RelationManager>();
  this->entityManager = std::make_shared<EntityManager>();
  this->grpManager =
      std::make_shared<GroupManager>(this->gen, this->relManager);

  this->relManager->reg(
      RelationType{.name = "marriage",
                   .cat = "person",
                   .symmetric = true,
                   .callback = [this](std::shared_ptr<Relation> rel) {
                     // fmt::print("rel: {}\n", *rel);
                     auto source = this->gen->get(rel->source);
                     auto target = this->gen->get(rel->target);
                     if (source->male) {
                       target->name.maid_last_name = target->name.last_name;
                       target->name.last_name = source->name.last_name;
                     } else {
                       source->name.maid_last_name = source->name.last_name;
                       source->name.last_name = target->name.last_name;
                     }
                     return rel;
                   }});

  this->relManager->reg(
      RelationType{.name = "family",
                   .cat = "person",
                   .symmetric = true,
                   .callback = [this](std::shared_ptr<Relation> rel) {
                     auto source = this->gen->get(rel->source);
                     auto target = this->gen->get(rel->target);
                     target->name.last_name = source->name.last_name;
                     return rel;
                   }});

  this->relManager->reg(
      RelationType{.name = "divorce",
                   .cat = "person",
                   .symmetric = true,
                   .callback = [this](std::shared_ptr<Relation> rel) {
                     // fmt::print("rel: {}\n", *rel);
                     auto source = this->gen->get(rel->source);
                     auto target = this->gen->get(rel->target);
                     if (source->male) {
                       target->name.last_name = target->name.maid_last_name;
                       target->name.maid_last_name = "";
                     } else {
                       source->name.last_name = source->name.maid_last_name;
                       source->name.maid_last_name = "";
                     }
                     return rel;
                   }});

  this->relManager->reg(
      RelationType{.name = "suspect",
                   .cat = "mystery",
                   .callback = [this](std::shared_ptr<Relation> rel) {
                     this->gen->property(
                         rel->target, "suspectInMurderOf",
                         this->entityManager->to(this->gen->get(rel->source)));
                     return rel;
                   }});

  this->relManager->reg(RelationType{
      .name = "parent",
      .cat = "person",
      .symmetric = false,
  });

  this->relManager->reg(RelationType{
      .name = "widow",
      .cat = "person",
      .symmetric = false,
  });

  this->relManager->reg(RelationType{
      .name = "sibling",
      .cat = "person",
      .symmetric = true,
  });
  this->relManager->reg(RelationType{
      .name = "friend",
      .cat = "person",
      .symmetric = true,
  });
  this->relManager->reg(RelationType{
      .name = "love",
      .cat = "person",
      .symmetric = false,
  });
  this->relManager->reg(RelationType{
      .name = "affair",
      .cat = "person",
      .symmetric = true,
  });
  this->relManager->reg(RelationType{
      .name = "housemate",
      .cat = "person",
      .symmetric = true,
  });
  this->relManager->reg(RelationType{
      .name = "clubmate",
      .cat = "person",
      .symmetric = true,
  });
  this->relManager->reg(RelationType{
      .name = "parishioner",
      .cat = "person",
      .symmetric = true,
  });
  this->relManager->reg(RelationType{
      .name = "classmate",
      .cat = "person",
      .symmetric = true,
  });
  this->relManager->reg(RelationType{
      .name = "coworker",
      .cat = "person",
      .symmetric = true,
  });

  this->relManager->reg(RelationType{
      .name = "boss",
      .cat = "person",
      .symmetric = false,
  });

  this->relManager->reg(RelationType{
      .name = "rival",
      .cat = "person",
      .symmetric = true,
  });
}

void CityGenerator::initOrgs() {
  this->grpManager->reg(GroupType{
      .name = "household",
      .cat = "person",
      .callback = [this](std::shared_ptr<Group> group,
                         std::shared_ptr<GroupRole> role) {
        if (role->name == "member") {
          this->relManager->add("housemate", role->members | to_vector());
        }
      }});

  this->grpManager->reg(GroupType{
      .name = "services",
      .titles = {"Services"},
      .cat = "singles",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            if (role->name != "freelancer") {
              this->relManager->add("rival", role->members | to_vector());
            }
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "plumber",
                  S(25, 55, 0.75f),
              },
              {
                  "electrician",
                  S(25, 55, 0.75f),
              },
              {
                  "garbage man",
                  S(25, 55, 0.75f),
              },
              {
                  "handyman",
                  S(20, 55, 0.75f),
              },
              {
                  "mechanic",
                  S(25, 55, 0.75f),
              },
              {
                  "IT specialist",
                  S(20, 45, std::nullopt),
              },
          },
      .roleCounts =
          {
              {"plumber", iS(1, 2)},
              {"electrician", iS(1, 2)},
              {"garbage man", iS(1, 2)},
              {"handyman", iS(1, 2)},
              {"IT specialist", iS(3, 6)},
          },
      .reputation =
          {
              {"plumber", 1},
              {"electrician", 1},
              {"garbage man", 1},
              {"handyman", 1},
              {"mechanic", 1},
              {"IT specialist", 2},
          },
      .wealth =
          {
              {"plumber", 2},
              {"electrician", 2},
              {"garbage man", 1},
              {"handyman", 1},
              {"mechanic", 1},
              {"IT specialist", 2},
          },
  });

  this->grpManager->reg(GroupType{
      .name = "singles",
      .titles = {"Entepreneurs"},
      .cat = "singles",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            if (role->name != "freelancer") {
              this->relManager->add("rival", role->members | to_vector());
            }
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "yoga trainer",
                  S(25, 55, std::nullopt),
              },
              {
                  "notary",
                  S(35, 65, std::nullopt),
              },
              {
                  "real estate agent",
                  S(35, 65, std::nullopt),
              },
              {
                  "reporter",
                  S(25, 65, std::nullopt),
              },
              {
                  "accountant",
                  S(35, 65, std::nullopt),
              },
              {
                  "exterminator",
                  S(35, 65, std::nullopt),
              },
              {
                  "freelancer",
                  S(18, 55, std::nullopt),
              },
          },
      .roleCounts =
          {
              {"freelancer", iS(5, 20)},
          },
      .reputation =
          {
              {"yoga trainer", 1},
              {"notary", 5},
              {"real estate agent", 1},
              {"reporter", 2},
              {"accountant", 3},
              {"extrminator", 0},
              {"freelancer", 0},
          },
      .wealth =
          {
              {"yoga trainer", 1},
              {"notary", 4},
              {"real estate agent", 2},
              {"reporter", 1},
              {"accountant", 3},
              {"extrminator", 1},
              {"freelancer", 1},
          },
      .location_type = "office",
      .location_multi = true,
      .location_per_role = true,
  });

  this->grpManager->reg(GroupType{
      .name = "police",
      .titles = {"Police"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());

            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "sheriff",
                  S(30, 65, 0.75f),
              },
              {
                  "deputy",
                  S(20, 55, 0.75f),
              },
          },
      .roleCounts =
          {
              {"sheriff", intSpec{1}},
              {"deputy", intSpec{std::pair<int, int>{0, 3}}},
          },
      .roleRels = {{"sheriff", {{"deputy"}, {"boss", "coworker"}}}},

      .reputation =
          {
              {"sheriff", 5},
              {"deputy", 3},
          },
      .wealth =
          {
              {"sheriff", 2},
              {"deputy", 1},
          },
      .location_type = "other",
  });

  this->grpManager->reg(GroupType{
      .name = "post",
      .titles = {"USPS office", "Post office", "US Post"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "head",
                  S(30, 65, std::nullopt),
              },
              {
                  "postman",
                  S(20, 55, std::nullopt),
              },
          },
      .roleCounts =
          {
              {"head", intSpec{1}},
              {"postman", intSpec{std::pair<int, int>{0, 2}}},
          },
      .roleRels = {{"head", {{"postman"}, {"boss", "coworker"}}}},
      .reputation =
          {
              {"head", 3},
              {"postman", 2},
          },
      .wealth =
          {
              {"head", 2},
              {"postman", 1},
          },
      .location_type = "other",
  });

  this->grpManager->reg(GroupType{
      .name = "clinic",
      .titles = {"Med clinic", "Vet clinic", "Dental clinic"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "doctor",
                  S(35, 65, std::nullopt),
              },
              {
                  "nurse",
                  S(25, 55, false),
              },
          },
      .roleCounts =
          {
              {"doctor", intSpec{1}},
              {"nurse", intSpec{1}},
          },
      .roleRels = {{"doctor", {{"nurse"}, {"boss", "coworker"}}}},
      .reputation =
          {
              {"doctor", 3},
              {"nurse", 1},
          },
      .wealth =
          {
              {"doctor", 3},
              {"nurse", 1},
          },
      .location_type = "commerical",
      .location_multi = true,
  });

  this->grpManager->reg(GroupType{
      .name = "FBI",
      .titles = {"FBI"},
      .cat = "org",
  });

  this->grpManager->reg(GroupType{
      .name = "goverment",
      .titles = {"Goverment"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "mayor",
                  S(35, 65, std::nullopt),
              },
              {
                  "secretary",
                  S(25, 55, 0.25f),
              },
          },
      .roleCounts =
          {
              {"mayor", intSpec{1}},
              {"secretary", intSpec{1}},
          },
      .roleRels = {{"mayor", {{"secretary"}, {"boss", "coworker"}}}},
      .reputation =
          {
              {"mayor", 5},
              {"secretary", 1},
          },
      .wealth =
          {
              {"mayor", 3},
              {"secretary", 2},
          },
      .location_type = "office",
      .location_multi = true,
  });

  this->grpManager->reg(GroupType{
      .name = "salon",
      .titles = {"Beauty salon"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "owner",
                  S(30, 65, false),
              },
              {
                  "specialist",
                  S(25, 55, 0.25f),
              },
          },
      .roleCounts =
          {
              {"owner", intSpec{1}},
              {"specialist", iS(0, 2)},
          },
      .roleRels = {{"owner", {{"specialist"}, {"coworker"}}}},
      .reputation =
          {
              {"owner", 2},
              {"specialist", 2},
          },
      .wealth =
          {
              {"owner", 2},
              {"specialist", 2},
          },
      .location_type = "commerical",
      .location_multi = true,
  });

  this->grpManager->reg(GroupType{
      .name = "cafe",
      .titles = {"Diner", "Restaurant", "Cafe", "Bistro"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "owner",
                  S(30, 65, std::nullopt),
              },
              {
                  "chief",
                  S(30, 65, std::nullopt),
              },
              {
                  "waiter",
                  S(18, 40, std::nullopt),
              },
          },
      .roleCounts =
          {
              {"chief", intSpec{1}},
              {"owner", iS(0, 1)},
              {"waiter", iS(1, 2)},
          },
      .roleRels =
          {
              {"owner", {{"chief"}, {"boss", "coworker"}}},
              {"chief", {{"waiter"}, {"coworker"}}},
              {"owner", {{"waiter"}, {"boss", "coworker"}}},
          },
      .family_business = true,
      .reputation =
          {
              {"owner", 2},
              {"chief", 2},
              {"waiter", 1},
          },
      .wealth =
          {
              {"owner", 3},
              {"chief", 3},
              {"waiter", 1},
          },
      .location_type = "commerical",
  });

  this->grpManager->reg(GroupType{
      .name = "manufacturer",
      .titles = {"Factory"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
            if (role->name == "security") {
              for (auto m : role->members) {
                this->gen->property(m, "weapon", "gun");
              }
            }
          },
      .roleSpecs =
          {
              {
                  "owner",
                  S(30, 65, std::nullopt),
              },
              {
                  "manager",
                  S(30, 65, std::nullopt),
              },
              {
                  "worker",
                  S(18, 55, std::nullopt),
              },
              {
                  "security",
                  S(25, 65, true),
              },
          },
      .roleCounts =
          {
              {"owner", intSpec{1}},
              {"manager", iS(0, 3)},
              {"worker", iS(10, 50)},
              {"security", intSpec{1}},
          },
      .roleRels =
          {
              {"owner",
               {{"manager", "worker", "security"}, {"boss", "coworker"}}},
              {"manager", {{"worker"}, {"boss", "coworker"}}},
              {"manager", {{"security"}, {"coworker"}}},
              {"security", {{"worker"}, {"coworker"}}},
          },
      .reputation =
          {
              {"owner", 4},
              {"manager", 2},
              {"worker", 0},
              {"security", 0},
          },
      .wealth =
          {
              {"owner", 5},
              {"manager", 3},
              {"worker", 2},
              {"security", 1},
          },
      .location_type = "industrial",
  });

  this->grpManager->reg(GroupType{
      .name = "school",
      .titles = {"School"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            if (role->name != "student") {
              this->relManager->add("coworker", role->members | to_vector());
              setOccupation(group, role);
            } else {
              this->relManager->add("classmate", role->members | to_vector());
            }
          },
      .roleSpecs =
          {
              {
                  "director",
                  S(30, 65, std::nullopt),
              },
              {
                  "teacher",
                  S(30, 65, std::nullopt),
              },
              {
                  "janitor",
                  S(18, 55, std::nullopt),
              },
              {
                  "security",
                  S(25, 65, true),
              },
              {
                  "student",
                  S(7, 18, std::nullopt),
              },
          },
      .roleCounts =
          {
              {"director", intSpec{1}},
              {"teacher", iS(1, 3)},
              {"janitor", intSpec{1}},
              {"security", intSpec{1}},
              {"student", intSpec{50}},
          },
      .roleRels =
          {
              {"director",
               {{"teacher", "janitor", "security"}, {"boss", "coworker"}}},
              {"teacher", {{"janitor", "secutity"}, {"coworker"}}},
              {"security", {{"janitor"}, {"coworker"}}},
          },
      .reputation =
          {
              {"director", 2},
              {"teacher", 3},
              {"janitor", 0},
              {"security", 0},
          },
      .wealth =
          {
              {"director", 2},
              {"teacher", 2},
              {"janitor", 1},
              {"security", 1},
          },
      .location_type = "other",
  });

  this->grpManager->reg(GroupType{
      .name = "construction",
      .titles = {"Construction company"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "owner",
                  S(30, 65, std::nullopt),
              },
              {
                  "worker",
                  S(25, 55, 0.85f),
              },
          },
      .roleCounts =
          {
              {"owner", intSpec{1}},
              {"worker", iS(3, 6)},
          },
      .roleRels = {{"owner", {{"worker"}, {"coworker"}}}},
      .reputation =
          {
              {"owner", 1},
              {"worker", 1},
          },
      .wealth =
          {
              {"owner", 2},
              {"worker", 1},
          },
      .location_type = "industrial",
  });

  this->grpManager->reg(GroupType{
      .name = "cleaning",
      .titles = {"Cleaning service"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
          },
      .roleSpecs =
          {
              {
                  "owner",
                  S(30, 65, std::nullopt),
              },
              {
                  "worker",
                  S(25, 55, 0.15f),
              },
          },
      .roleCounts =
          {
              {"owner", intSpec{1}},
              {"worker", iS(2, 4)},
          },
      .roleRels = {{"owner", {{"worker"}, {"coworker"}}}},
      .reputation =
          {
              {"owner", 1},
              {"worker", 1},
          },
      .wealth =
          {
              {"owner", 2},
              {"worker", 1},
          },
      .location_type = "industrial",
      .location_multi = true,
  });

  this->grpManager->reg(GroupType{
      .name = "corp",
      .titles = {"Corporation branch"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            this->relManager->add("coworker", role->members | to_vector());
            setOccupation(group, role);
            if (role->name == "security" || role->name == "chief-security") {
              for (auto m : role->members) {
                this->gen->property(m, "weapon", "gun");
              }
            }
          },
      .roleSpecs =
          {
              {"chief-manager",
               PersonSpec{
                   .age = intSpec{std::pair<int, int>{40, 55}},
                   .dead = boolSpec{false},
                   .local = boolSpec{false},
                   .occupation = boolSpec{false},
               }},
              {
                  "manager",
                  S(30, 65, std::nullopt),
              },
              {
                  "worker",
                  S(20, 65, std::nullopt),
              },
              {
                  "chief-security",
                  S(35, 45, std::nullopt),
              },
              {
                  "security",
                  S(25, 45, 0.9f),
              },
          },
      .roleCounts =
          {
              {"chief-manager", intSpec{1}},
              {"manager", iS(3, 5)},
              {"worker", iS(20, 60)},
              {"chief-security", intSpec{1}},
              {"security", intSpec{25}},
          },
      .roleRels =
          {
              {"chief-manager",
               {{"manager", "worker", "chief-security", "security"},
                {"boss", "coworker"}}},
              {"manager", {{"worker"}, {"boss", "coworker"}}},
              {"chief-security", {{"security"}, {"boss", "coworker"}}},
              {"security", {{"worker"}, {"coworker"}}},
          },
      .reputation =
          {
              {"chief-manager", 5},
              {"manager", 3},
              {"worker", 0},
              {"chief-security", 2},
              {"security", 0},
          },
      .wealth =
          {
              {"chief-manager", 6},
              {"manager", 4},
              {"worker", 3},
              {"chief-security", 3},
              {"security", 2},
          },
      .location_type = "office",
      .location_per_role = true,
  });

  this->grpManager->reg(GroupType{
      .name = "church",
      .titles = {"Church"},
      .cat = "org",
      .callback =
          [&](std::shared_ptr<Group> group, std::shared_ptr<GroupRole> role) {
            if (role->name != "member") {
              setOccupation(group, role);
            }
          },
      .roleSpecs =
          {
              {
                  "priest",
                  S(35, 65, std::nullopt),
              },
              {
                  "secretary",
                  S(25, 55, 0.25f),
              },
              {
                  "member",
                  S(18, 90, std::nullopt),
              },
          },
      .roleCounts =
          {
              {"priest", intSpec{1}},
              {"secretary", intSpec{1}},
              {"member", iS(10, 50)},
          },
      .reputation =
          {
              {"priest", 5},
              {"secretary", 1},
          },
      .wealth =
          {
              {"priest", 1},
              {"secretary", 1},
          },
      .location_type = "other",
  });

  this->grpManager->reg(GroupType{
      .name = "hobby",
      .titles = {"Hobby club"},
      .cat = "org",
      .callback = [&](std::shared_ptr<Group> group,
                      std::shared_ptr<GroupRole> role) {},
      .roleSpecs =
          {
              {
                  "president",
                  S(35, 85, std::nullopt),
              },
              {
                  "vice-president",
                  S(35, 85, std::nullopt),
              },
              {
                  "treasurer",
                  S(35, 85, std::nullopt),
              },
              {
                  "secretary",
                  S(25, 75, std::nullopt),
              },
              {
                  "member",
                  S(18, 90, std::nullopt),
              },
          },
      .roleCounts =
          {
              {"president", intSpec{1}},
              {"vice-president", intSpec{1}},
              {"treasurer", intSpec{1}},
              {"member", iS(10, 50)},
          },
      .reputation =
          {
              {"president", 1},
          },
  });
}
