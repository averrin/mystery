#pragma once
#include "../../target/cxxbridge/src/common.rs.h"
#include "generator.hpp"
#include "generators/person_gen.hpp"
#include "relation_manager.hpp"
#include "specs/common_specs.hpp"
#include "specs/person_spec.hpp"
#include "utils.hpp"
#include <memory>

typedef std::function<void(std::shared_ptr<Group>, std::shared_ptr<GroupRole>)>
    groupCb;
struct GroupType {
  std::string name = "";
  std::vector<std::string> titles;
  std::string cat = "";
  groupCb callback;
  std::map<std::string, PersonSpec> roleSpecs;
  std::map<std::string, intSpec> roleCounts;
  std::map<std::string,
           std::pair<std::vector<std::string>, std::vector<std::string>>>
      roleRels;
  bool family_business = false;
  intSpec workers = intSpec{0};
  std::map<std::string, int> reputation;
  std::map<std::string, int> wealth;
  std::string location_type = "none";
  bool location_multi = false;
  bool location_per_role = false;
};

class GroupManager : public Container<Group> {
public:
  std::map<std::string, GroupType> grpTypes;
  std::shared_ptr<Container<GroupRole>> roles;
  std::shared_ptr<PersonGenerator> gen;
  std::shared_ptr<RelationManager> relManager;

  GroupManager(std::shared_ptr<PersonGenerator> g,
               std::shared_ptr<RelationManager> r)
      : Container() {
    roles = std::make_shared<Container<GroupRole>>();
    this->gen = g;
    this->relManager = r;
  }

  void reg(GroupType grpType) { this->grpTypes[grpType.name] = grpType; }

  std::shared_ptr<Group> add(int role_id, std::vector<int> ids) {
    auto role = this->roles->get(role_id);
    for (auto i : ids) {
      role->members.push_back(i);
    }
    auto group = this->get(role->group_id);
    return group;
  }

  std::shared_ptr<Group> populate(std::string grpTypeName) {
    if (this->grpTypes.find(grpTypeName) == this->grpTypes.end()) {
      throw std::runtime_error("undefined group type");
    }
    auto grpType = this->grpTypes[grpTypeName];
    std::string name = "";
    if (!grpType.titles.empty()) {
      name = grpType.titles.at(Random::get<int>(0, grpType.titles.size() - 1));
    }
    auto group = this->add(name, grpTypeName);
    // for (auto rc : grpType.roleCounts) {
    for (auto _rc : grpType.roleSpecs) {
      auto rc = std::pair<std::string, intSpec>{_rc.first, intSpec{1}};
      if (grpType.roleCounts.find(_rc.first) != grpType.roleCounts.end()) {
        rc = {_rc.first, grpType.roleCounts.at(_rc.first)};
      }
      for (auto n = 0; n < rc.second.get(); n++) {
        auto spec = grpType.roleSpecs[rc.first];
        auto p = this->gen->find(spec);
        if (p) {
          this->add(group->id, rc.first, {(*p)->id});
          auto _w = this->gen->property((*p)->id, "wealth");
          auto w = _w ? std::get<float>(*_w) : 0;
          auto _r = this->gen->property((*p)->id, "reputation");
          auto r = _r ? std::get<float>(*_r) : 0;
          this->gen->property((*p)->id, "wealth", grpType.wealth[rc.first] + w);
          this->gen->property((*p)->id, "reputation",
                              grpType.reputation[rc.first] + r);
        } else {
          fmt::print(fmt::runtime("!!! cannot find person {} for group {}\n"),
                     spec, name);
          break;
        }
      }
    }
    for (auto [rn, rels] : grpType.roleRels) {
      auto as = this->getMembers(group->id, rn);
      for (auto a : as) {
        for (auto bs : rels.first) {
          auto ps = this->getMembers(group->id, bs);
          for (auto p : ps) {
            for (auto rel : rels.second) {
              this->relManager->add(rel, a, p);
            }
          }
        }
      }
    }
    return group;
  }

  std::vector<int> getMembers(int id) {
    std::vector<int> ids;
    auto roles = this->roles->items |
                 std::views::filter([&](std::shared_ptr<GroupRole> r) {
                   return r->group_id == id;
                 }) |
                 to_vector();
    for (auto role : roles) {
      std::copy(role->members.begin(), role->members.end(),
                std::back_inserter(ids));
    }

    return ids;
  }

  std::vector<int> getMembers(int id, std::string grpRoleName) {
    std::vector<int> ids;
    auto roles = this->roles->items |
                 std::views::filter([&](std::shared_ptr<GroupRole> r) {
                   return r->name == grpRoleName && r->group_id == id;
                 }) |
                 to_vector();
    if (!roles.empty()) {
      auto role = roles[0];
      std::copy(role->members.begin(), role->members.end(),
                std::back_inserter(ids));
    }

    return ids;
  }

  std::shared_ptr<Group> add(int id, std::string grpRoleName,
                             std::vector<int> ids) {
    auto group = this->get(id);
    auto role = std::make_shared<GroupRole>(GroupRole{
        .group_id = group->id,
        .name = grpRoleName,
    });
    auto roles = this->roles->items |
                 std::views::filter([&](std::shared_ptr<GroupRole> r) {
                   return r->name == grpRoleName && r->group_id == group->id;
                 }) |
                 to_vector();
    if (!roles.empty()) {
      role = roles[0];
    } else {
      auto role_id = this->roles->save(role);
      group->roles.push_back(role_id);
    }
    std::copy(ids.begin(), ids.end(), std::back_inserter(role->members));
    auto grpType = this->grpTypes[std::string(group->group_type)];
    if (grpType.callback) {
      grpType.callback(group, role);
    }
    return group;
  }

  std::shared_ptr<Group> add(std::string name, std::string grpTypeName) {
    if (this->grpTypes.find(grpTypeName) == this->grpTypes.end()) {
      throw std::runtime_error("undefined group type");
    }
    auto grpType = this->grpTypes[grpTypeName];

    auto group = std::make_shared<Group>(Group{
        .name = name,
        .group_type = grpTypeName,
    });
    auto group_id = this->save(group);
    return group;
  }

  std::shared_ptr<Group> add(std::string name, std::string grpTypeName,
                             std::string grpRoleName, std::vector<int> ids) {
    auto group = this->add(name, grpTypeName);
    this->add(group->id, grpRoleName, ids);
    return group;
  }
};
