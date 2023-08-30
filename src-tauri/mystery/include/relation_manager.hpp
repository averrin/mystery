#pragma once
#include "generator.hpp"
#include "../../target/cxxbridge/src/common.rs.h"
#include "utils.hpp"

typedef std::function<void(std::shared_ptr<Relation>)> relationCb;
struct RelationType {
  std::string name = "";
  std::string cat = "";
  bool symmetric = false;
  relationCb callback;
};

class RelationManager : public Container<Relation> {
public:
  std::map<std::string, RelationType> relTypes;

  auto getRelations(int id) {
    return this->items | views::filter([id](std::shared_ptr<Relation> r) {
             return r->target == id || r->source == id;
           });
  }

  auto getRelations(std::string relTypeName, int id, bool only_active = false) {
    if (this->relTypes.find(relTypeName) == this->relTypes.end()) {
      throw std::runtime_error(fmt::format("undefined relation: {}", relTypeName));
    }
    return this->getRelations(id) |
           views::filter([&](std::shared_ptr<Relation> r) {
             return r->rel_type == relTypeName && (!only_active || r->active);
           }) | to_vector();
  }

  auto getSources(std::string relTypeName, int id, bool recursive = false, bool only_active = false) {
    auto rels = this->getRelations(relTypeName, id, only_active);
    auto items = rels | views::transform([](std::shared_ptr<Relation> r) {
                   return r->source;
                 }) |
                 unique();
    items =
        items | views::filter([id](int i) { return i != id; }) | to_vector();
    if (!recursive)
      return items;
    auto _items = items;
    for (auto p : _items) {
      auto res = getSources(relTypeName, p, recursive) | unique();
      std::copy(res.begin(), res.end(), std::back_inserter(items));
    }
    return items;
  }

  auto getTargets(std::string relTypeName, int id, bool recursive = false, bool only_active = false) {
    auto rels = this->getRelations(relTypeName, id, only_active);
    auto items = rels | views::transform([](std::shared_ptr<Relation> r) {
                   return r->target;
                 }) |
                 unique();
    items =
        items | views::filter([id](int i) { return i != id; }) | to_vector();
    if (!recursive)
      return items;
    auto _items = items;
    for (auto p : _items) {
      auto res = getTargets(relTypeName, p, recursive) | unique();
      std::copy(res.begin(), res.end(), std::back_inserter(items));
    }
    return items;
  }

  auto getSymmetric(std::string relTypeName, int id, bool only_active = false) {
    auto s = this->getSources(relTypeName, id, false, only_active) | to_vector();
    auto t = this->getTargets(relTypeName, id, false, only_active) | to_vector();
    std::vector<int> out;
    out.reserve(s.size() + t.size());
    std::ranges::merge(s, t, std::back_inserter(out));
    return out | views::filter([id](int i) { return i != id; }) | unique() | to_vector();
  }

  void reg(RelationType relType) { this->relTypes[relType.name] = relType; }

  std::shared_ptr<Relation> add(std::string relTypeName, int source,
                                int target) {
    if (this->relTypes.find(relTypeName) == this->relTypes.end()) {
      throw std::runtime_error("undefined relation type");
    }
    auto relType = this->relTypes[relTypeName];

    auto rel = std::make_shared<Relation>(Relation{
        .source = source,
        .target = target,
        .rel_type = relTypeName,
        .rel_cat = relType.cat,
        .symmetric = relType.symmetric,
        .active = true,
    });
    if (relType.callback) {
      relType.callback(rel);
    }
    return this->get(this->save(rel));
  }

  std::vector<std::shared_ptr<Relation>> add(std::string relTypeName,
                                             std::vector<int> ids) {
    if (this->relTypes.find(relTypeName) == this->relTypes.end()) {
      throw std::runtime_error("undefined relation");
    }
    auto relType = relTypes[relTypeName];

    std::vector<std::shared_ptr<Relation>> res;
    for (auto id1 : ids) {
      for (auto id2 : ids | std::views::reverse) {
        if (id1 == id2)
          break;
        res.push_back(this->add(relTypeName, id1, id2));
      }
    }
    return res;
  }
};

template <> struct fmt::formatter<Relation> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  auto format(Relation rel, format_context &ctx) const {
    return fmt::format_to(ctx.out(), "[{}] {}: {} -> {}", rel.id,
                          std::string(rel.rel_type), rel.source, rel.target);
  };
};
