#pragma once
#include "../../../target/cxxbridge/src/common.rs.h"
#include "../generator.hpp"
#include "../specs/common_specs.hpp"
#include "../utils.hpp"
#include <cctype>

struct LocationSpec {
  std::string name;
  std::string cat;
  std::vector<std::string> titles;
  std::string location_type;
  intSpec count;
};

class LocationGenerator : public Container<Location> {
  std::shared_ptr<RManager> rm;

public:
  std::shared_ptr<Container<Street>> streets;
  std::shared_ptr<Container<Building>> buildings;
  std::map<int, std::vector<int>> streetBuildings;
  std::map<int, int> buildingLastApartment;
  // building types: single-family, apartments, office, commercial, industrial

  LocationGenerator(std::shared_ptr<RManager> _rm) : Container(), rm(_rm) {
    streets = std::make_shared<Container<Street>>();
    buildings = std::make_shared<Container<Building>>();
    for (auto n = 0; n < intSpec{std::pair<int, int>{20, 30}}.get(); n++) {
      auto street = std::make_shared<Street>(Street{
          .name = this->rm->get<std::string>("street", true),
          .street_type = "",
      });
      streets->save(street);
    }
  }

  std::shared_ptr<Location> get(std::string type, bool multi) {
    if (Random::get<bool>(0.1)) {
      auto e = this->get(type, multi);
      e->name = "Empty";
    }
    if (type == "residential") {
      auto m = "single";
      if (multi) {
        m = "multi";
      }
      type = fmt::format("{}-{}", type, m);
    }

    auto street_type = type;
    if (type == "office" || type == "commerical") {
      street_type = "downtown";
    }
    if (type == "other") {
      street_type = Random::get<std::string>(
          {"downtown", "residential-single", "residential-multi"});
    }

    auto max_apts = 12;
    auto max_houses = Random::get<int>(15, 20);

    auto apartment = 0;
    auto _street = streets->items | std::views::filter([&](auto s) {
                     return s->street_type == street_type &&
                            streetBuildings[s->id].size() < max_houses;
                   });
    std::shared_ptr<Street> street;
    if (_street.empty()) {
      auto fs =
          streets->items |
          std::views::filter([&](auto s) { return s->street_type == ""; }) |
          to_vector();
      street = fs.at(Random::get<int>(0, fs.size() - 1));
    } else {
      street = (_street | to_vector()).at(0);
    }
    street->street_type = street_type;
    auto addr_count = streetBuildings[street->id].size();
    std::shared_ptr<Building> building;
    if (!multi) {
      building = std::make_shared<Building>(Building{
          .building_type = type,
          .street = street->id,
          .number = static_cast<int32_t>(addr_count + 1),
      });
      buildings->save(building);
      streetBuildings[street->id].push_back(building->id);
    } else {
      apartment = 1;
      auto _buildings =
          streetBuildings[street->id] | std::views::filter([&](auto a) {
            return this->buildings->get(a)->building_type == type &&
                   buildingLastApartment[a] < max_apts;
          }) |
          to_vector();
      if (_buildings.empty()) {
        building = std::make_shared<Building>(Building{
            .building_type = type,
            .street = street->id,
            .number = static_cast<int32_t>(addr_count + 1),
        });
        buildings->save(building);
        streetBuildings[street->id].push_back(building->id);
      } else {
        building = this->buildings->get(_buildings.at(0));
      }
      apartment = buildingLastApartment[building->id] + 1;
    }

    buildingLastApartment[building->id] = apartment;
    auto location = std::make_shared<Location>(Location{
        .building = building->id,
    });
    if (apartment > 0) {
      location->apartment = apartment;
    }
    this->save(location);
    return location;
  }
};
