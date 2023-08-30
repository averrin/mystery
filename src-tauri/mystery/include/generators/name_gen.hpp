#pragma once
#include "../generator.hpp"
#include "../randomizer.hpp"
#include "../specs/names_spec.hpp"
#include "../../../target/cxxbridge/src/common.rs.h"
#include <fmt/core.h>

class NameGenerator : public Generator<Names, NamesSpec> {
public:
  NameGenerator(std::shared_ptr<RManager> _rm,
                std::shared_ptr<DbManager> _db)
      : Generator(_rm, _db) {
    std::string tableName = "names";
    auto fields = std::vector<std::string>{
        "id INTEGER PRIMARY KEY", "first_name TEXT",     "middle_name TEXT",
        "last_name TEXT",         "maid_last_name TEXT",
    };
    this->db->createTable(tableName, fields);
  // fmt::print("{}\n", this->db->getFilename());
    // SQLite::Statement query{*this->db, "DROP TABLE IF EXISTS ?"};
    // query.bind(1, tableName);
    // query.exec();
    // _db->exec(fmt::format(fmt::runtime("DROP TABLE IF EXISTS {}"),
    // tableName)); this->db->exec(fmt::format(fmt::runtime("CREATE TABLE {}
    // ({})"), tableName, fmt::join(fields, ", ")));
  }
  std::shared_ptr<Names> get(NamesSpec);

  std::shared_ptr<Names> get() { return Generator::get(); }

  std::shared_ptr<Names> get(int i) { return Container::get(i); }

  std::optional<std::shared_ptr<Names>> find(NamesSpec);
  std::vector<std::shared_ptr<Names>> findAll(NamesSpec);
};

template <> struct fmt::formatter<Names> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  auto format(Names name, format_context &ctx) const {
    if (name.middle_name == "") {
      return fmt::format_to(ctx.out(), fmt::runtime("{} {}"),
                            std::string(name.first_name),
                            std::string(name.last_name));
    } else {
      return fmt::format_to(
          ctx.out(), fmt::runtime("{} {}. {}"), std::string(name.first_name),
          std::string(name.middle_name).at(0), std::string(name.last_name));
    }
  };
};
