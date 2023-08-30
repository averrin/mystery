#pragma once
#include "randomizer.hpp"
// #include "sqlite_orm.h"
#include <memory>
#include <variant>
#include <functional>
#include <optional>
#include "db_manager.hpp"

template <class T> class Container {
protected:
  int count = 0;
  int id() {
    auto id = count;
    ++count;
    return id;
  }

public:
  std::vector<std::shared_ptr<T>> items;

  std::shared_ptr<T> get(int id) {
    return items.at(id);
  };

  int save(std::shared_ptr<T> item) {
    item->id = this->id();
    items.push_back(item);
    return item->id;
  };
};


template <class T, class S> class Generator : public Container<T> {
public:
  //TODO: make variadic
  typedef std::variant<std::function<S(std::shared_ptr<T>)>, std::function<S(std::shared_ptr<T>,std::shared_ptr<T>)>> specFunc;
protected:
  std::shared_ptr<RManager> rm;
  std::map<std::string, specFunc> specs;
  std::shared_ptr<DbManager> db;

public:
  Generator(std::shared_ptr<RManager> _rm, std::shared_ptr<DbManager> _db) : rm(_rm) {
    this->db = db;
  }

  std::shared_ptr<T> get() { return this->get(S{}); }

  virtual std::shared_ptr<T> get(S) = 0;
  virtual std::optional<std::shared_ptr<T>> find(S) = 0;
  virtual std::vector<std::shared_ptr<T>> findAll(S) = 0;

  void addSpec(std::string k, specFunc f) {
    specs[k] = f;
  }

  S getSpec(std::string k, std::shared_ptr<T> a) {
    return std::get<std::function<S(std::shared_ptr<T>)>>(specs[k])(a);
  }

  S getSpec(std::string k, std::shared_ptr<T> a, std::shared_ptr<T> b) {
    return std::get<std::function<S(std::shared_ptr<T>,std::shared_ptr<T>)>>(specs[k])(a,b);
  }
};
