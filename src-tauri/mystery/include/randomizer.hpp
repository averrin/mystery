#pragma once
#include <any>
#include <fstream>
#include <iostream>
#include <memory>
#include "db_manager.hpp"
#include "json.hpp"
using json = nlohmann::json;
#include "random.hpp"
using Random = effolkronium::random_static;

class Randomizer {
  public:
  Randomizer(std::string path) {

    // auto L = Logger::getInstance();
    // L.info("{}", path);
    // std::cout << path << std::endl;
    std::ifstream file(path);
    auto data = json::parse(file);
    // std::cout << "parsed: " << data.size() << std::endl;
    for (auto item : data) {
      items.push_back(item);
    }
  }
  
  std::vector<json> items;
  // json data;
  std::vector<int> taken;

  json get(bool uniq=false) {
    if (taken.size() == items.size()) {
      reset();
    }
    auto i = Random::get<int>(0, items.size()-1);
    if (uniq) {
      while (std::find(taken.begin(), taken.end(), i) != taken.end()) {
        i = Random::get<int>(0, items.size()-1);
      }
    }
    taken.push_back(i);
    return items.at(i);
  }

  void reset() {
    taken.clear();
  }
};

class RManager : public std::enable_shared_from_this<RManager> {
  int _seed;
  std::shared_ptr<DbManager> db;
public:
  std::map<std::string, std::shared_ptr<Randomizer>> randomizers;
  std::map<std::string, std::any> generators;
  int currentYear;

RManager(std::shared_ptr<DbManager> _db) : db(_db) {
  }


  int seed() {
    return _seed;
  }

  void seed(int s) {
    _seed = s;
    Random::seed(_seed);
  }

  template<typename T>
  auto getGen(std::string key) {
    return std::any_cast<std::shared_ptr<T>>(generators.at(key));
  }

  template<typename T>
  auto reg(std::string key) {
    auto gen = std::make_shared<T>(shared_from_this(), this->db);
    generators[key] = std::any(gen);
    return gen;
  }

  std::shared_ptr<Randomizer> reg(std::string key, std::string path) {

    // std::cout << key << " : " << path << std::endl;
    randomizers[key] = std::make_shared<Randomizer>(path);
    return randomizers[key];
  }

  json get(std::string key, bool uniq=false) {
    return randomizers.at(key)->get(uniq);
  }

  template<typename T>
  auto get(std::string key, bool uniq=false) {
    return get(key, uniq).get<T>();
  }
};
