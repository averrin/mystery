#pragma once
#include "../../../target/cxxbridge/src/common.rs.h"
#include "../generator.hpp"
#include "../specs/common_specs.hpp"
#include "../utils.hpp"
#include <cctype>

using namespace std::ranges;

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
  std::map<std::string, std::vector<std::string>> landmarks = {
      {"downtown", {"park", "monument"}},
      {"residential-single", {"park", "sport field"}},
      {"residential-multi", {"park", "sport field"}},
      {"industrial", {"park"}},
      {"commercial", {"park", "market square"}},
  };

  int rootPhoneCode;
  std::map<int, std::pair<int, int>> streetPhones;

  LocationGenerator(std::shared_ptr<RManager> _rm) : Container(), rm(_rm) {
    rootPhoneCode = Random::get<int>(100, 999);
    auto sc = Random::get<int>(100, 700);
    streets = std::make_shared<Container<Street>>();
    buildings = std::make_shared<Container<Building>>();
    for (auto n = 0; n < intSpec{std::pair<int, int>{20, 30}}.get(); n++) {
      auto street = std::make_shared<Street>(Street{
          .name = this->rm->get<std::string>("street", true),
          .street_type = "",
      });
      auto sid = streets->save(street);
      sc = sc + Random::get<int>(1, 5);
      streetPhones[sid] =
          std::pair<int, int>{sc, 1000 + Random::get<int>(1, 500)};
    }
  }

  std::shared_ptr<Location> get(int id) { return Container::get(id); }

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
    if (type == "office" || type == "commercial") {
      street_type = "downtown";
    }
    if (type == "other") {
      street_type = Random::get<std::string>(
          {"downtown", "residential-single", "residential-multi"});
    }

    auto max_apts = 12;
    auto max_houses = Random::get<int>(15, 20);

    auto apartment = 0;
    auto _street = streets->items | views::filter([&](auto s) {
                     return s->street_type == street_type &&
                            streetBuildings[s->id].size() < max_houses;
                   });
    std::shared_ptr<Street> street;
    if (_street.empty()) {
      auto fs = streets->items |
                views::filter([&](auto s) { return s->street_type == ""; }) |
                to_vector();
      street = fs.at(Random::get<int>(0, fs.size() - 1));

      for (auto n = 0; n < intSpec{std::pair<int, int>{1, 3}}.get(); n++) {
        auto lms = landmarks.at(street_type);
        auto landmark = lms.at(Random::get<int>(0, lms.size() - 1));
        if (landmark == "park") {
          landmark = this->rm->get<std::string>("park", true);
        } else if (landmark == "monument") {
          landmark = this->rm->get<std::string>("monument", true);
        } else if (landmark == "sport field") {
          landmark = this->rm->get<std::string>("sport_field", true);
        }
        // else if (landmark == "market square") {
        //  landmark = this->rm->get<std::string>("market_square", true);
        // }
        auto loc = std::make_shared<Location>(Location{
            .name = landmark,
            .location_type = "landmark",
        });
        this->save(loc);
        street->landmarks.push_back(loc->id);
      }

      if (Random::get<bool>(0.8)) {
        auto loc = std::make_shared<Location>(Location{
            .name = fmt::format(fmt::runtime("Bus stop \"{}\""),
                                std::string(street->name)),
            .location_type = "bus stop",
        });
        this->save(loc);
        street->landmarks.push_back(loc->id);
      }
    } else {
      street = (_street | to_vector()).at(0);
    }
    street->street_type = street_type;
    auto bPhone = streetPhones[street->id].second + Random::get<int>(1, 10);
    streetPhones[street->id].second = bPhone;
    auto phone = fmt::format("({}) {}-{}", rootPhoneCode,
                             streetPhones[street->id].first, bPhone);
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
        .location_type = "building",
        .building = building->id,
        .phone = phone,
    });
    if (apartment > 0) {
      location->apartment = apartment;
    }
    this->save(location);
    return location;
  }
};
