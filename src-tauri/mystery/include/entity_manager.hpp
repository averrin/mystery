#pragma once
#include "generator.hpp"
#include "../../target/cxxbridge/src/common.rs.h"


class EntityManager : public Container<Entity>{
public:

  int to(std::shared_ptr<Person> p) {
    auto e = std::make_shared<Entity>(Entity{
      .e_type = "person",
      .e_id = p->id,
    });
    return this->save(e);
  }

  int to(std::shared_ptr<Relation> r) {
    auto e = std::make_shared<Entity>(Entity{
      .e_type = "relation",
      .e_id = r->id,
    });
    return this->save(e);
  }
};
