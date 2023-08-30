#pragma once
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <map>
#include <optional>
#include "../random.hpp"
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>
using Random = effolkronium::random_static;
using Random_t = effolkronium::random_local;

// template <class T, class S>
// class rndSpec {
// public:

// rndSpec(std::initializer_list<T> c) {

// }
// };
//
#include <random>

class boolSpec /*: rndSpec<bool, boolSpec>*/ {
public:
  typedef bool T;
  typedef boolSpec S;
  std::optional<std::variant<T, float, std::map<T, float>>> value;

  void set(T v) { value = v; }

bool randomBoolean(float p) {
  // static std::default_random_engine generator(std::random_device{}());
  // // With p = 0.5 you get equal probability for true and false
  // static std::bernoulli_distribution distribution(p);
  // return distribution(generator);
  return rand() > (RAND_MAX / 2);
}

  bool match(T mv) {
    if (!value) {
      return true;
    } else {
      auto v = value.value();
      if (std::holds_alternative<T>(v))
        return std::get<T>(v) == mv;
      if (std::holds_alternative<float>(v)) {
        return this->get() == mv;
      }
      if (std::holds_alternative<std::map<T, float>>(v)) {
        auto p = std::get<std::map<T, float>>(v);
        return p.find(mv) != p.end();
      }
      throw std::runtime_error("spec wasn't matched");
    }
  }

  T get_or(S spec) {
    if (!value) {
      return spec.get();
    } else {
      return this->get();
    }
  }

  T get() {
    if (!value) {
      return Random::get<T>();
    } else {
      auto v = value.value();
      if (std::holds_alternative<T>(v))
        return std::get<T>(v);
      if (std::holds_alternative<float>(v))
        return Random::get<T>(std::get<float>(v));
      if (std::holds_alternative<std::map<T, float>>(v))
        return (*Random::get<Random_t::weight>(std::get<std::map<T, float>>(v)))
            .first;
      throw std::runtime_error("spec wasn't matched");
    }
  }
};

template <> struct fmt::formatter<boolSpec> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  auto format(boolSpec spec, format_context &ctx) const {
    std::string res = "";
    if (!spec.value) {
      res = "<rnd>";
    } else {
      auto v = spec.value.value();
      if (std::holds_alternative<bool>(v))
        res = fmt::format("{}", std::get<bool>(v));
      if (std::holds_alternative<float>(v))
        res = fmt::format("<p:{}>", std::get<float>(v));
      if (std::holds_alternative<std::map<bool, float>>(v)) {
        auto p = std::get<std::map<bool, float>>(v);
        res += "{";
        for (auto [k, v] : p) {
          res += fmt::format("{}: {}, ", k, v);
        }
        res += "}";
      }
    }

    return fmt::format_to(ctx.out(), fmt::runtime("{}"), res);
  };
};

class intSpec {
public:
  typedef int T;
  typedef intSpec S;
  std::optional<std::variant<T, std::pair<T, T>, std::map<T, float>,
                             std::map<S, float>, std::vector<T>>>
      value;

  void set(T v) { value = v; }

  T get_or(S spec) {
    if (!value) {
      return spec.get();
    } else {
      return this->get();
    }
  }

  bool match(T mv) {
    if (!value) {
      return true;
    } else {
      auto v = value.value();
      if (std::holds_alternative<T>(v))
        return std::get<T>(v) == mv;
      if (std::holds_alternative<std::vector<T>>(v)) {
        auto p = std::get<std::vector<T>>(v);
        return std::find(p.begin(), p.end(), mv) != p.end();
      }
      if (std::holds_alternative<std::pair<T, T>>(v)) {
        auto p = std::get<std::pair<T, T>>(v);
        return mv >= p.first && mv <= p.second;
      }
      if (std::holds_alternative<std::map<T, float>>(v)) {
        auto p = std::get<std::map<T, float>>(v);
        return p.find(mv) != p.end();
      }
      throw std::runtime_error("spec wasn't matched");
    }
  }

  T get() {
    if (!value) {
      return Random::get<T>();
    } else {
      auto v = value.value();
      if (std::holds_alternative<T>(v))
        return std::get<T>(v);
      if (std::holds_alternative<std::pair<T, T>>(v)) {
        auto r = std::get<std::pair<T, T>>(v);
        return Random::get<T>(r.first, r.second);
      }
      if (std::holds_alternative<std::vector<T>>(v)) {
        auto r = std::get<std::vector<T>>(v);
        return r.at(Random::get<int>(r.size() - 1));
      }
      if (std::holds_alternative<std::map<T, float>>(v))
        return (*Random::get<Random_t::weight>(std::get<std::map<T, float>>(v)))
            .first;
      if (std::holds_alternative<std::map<S, float>>(v)) {
        auto spec =
            (*Random::get<Random_t::weight>(std::get<std::map<S, float>>(v)))
                .first;
        return spec.get();
      }
      throw std::runtime_error("spec wasn't matched");
    }
  }
};

template <> struct fmt::formatter<intSpec> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  auto format(intSpec spec, format_context &ctx) const {
    std::string res = "";
    if (!spec.value) {
      res = "<rnd>";
    } else {
      auto v = spec.value.value();
      if (std::holds_alternative<int>(v))
        res = fmt::format("{}", std::get<int>(v));
      if (std::holds_alternative<std::pair<int, int>>(v)) {
        auto r = std::get<std::pair<int, int>>(v);
        res = fmt::format("<p:[{}, {}]>", r.first, r.second);
      }
      if (std::holds_alternative<std::map<int, float>>(v)) {
        auto p = std::get<std::map<int, float>>(v);
        res += "{";
        for (auto [k, v] : p) {
          res += fmt::format("{}: {}, ", k, v);
        }
        res += "}";
      }
      if (std::holds_alternative<std::map<intSpec, float>>(v)) {
        auto p = std::get<std::map<intSpec, float>>(v);
        res += "{";
        for (auto [k, v] : p) {
          res += fmt::format(fmt::runtime("{}: {}, "), k, v);
        }
        res += "}";
      }
    }

    return fmt::format_to(ctx.out(), "{}", res);
  };
};

class stringSpec {
public:
  typedef std::string T;
  typedef stringSpec S;
  typedef std::optional<
      std::variant<std::string, std::map<std::string, float>,
                   std::map<stringSpec, float>, std::vector<std::string>>>
      stringSpecValue;

  stringSpecValue value;

  void set(std::string v) { value = v; }

  std::string get_or(stringSpec spec) {
    if (!value) {
      return spec.get();
    } else {
      return this->get();
    }
  }

  bool match(T mv) {
    if (!value) {
      return true;
    } else {
      auto v = value.value();
      if (std::holds_alternative<T>(v))
        return std::get<T>(v) == mv;
      if (std::holds_alternative<std::vector<T>>(v)) {
        auto p = std::get<std::vector<T>>(v);
        return std::find(p.begin(), p.end(), mv) != p.end();
      }
      if (std::holds_alternative<std::map<T, float>>(v)) {
        auto p = std::get<std::map<T, float>>(v);
        return p.find(mv) != p.end();
      }
      throw std::runtime_error("spec wasn't matched");
    }
  }

  std::string get() {
    if (!value) {
      return "";
    } else {
      auto v = value.value();
      if (std::holds_alternative<std::string>(v))
        return std::get<std::string>(v);
      if (std::holds_alternative<std::map<std::string, float>>(v))
        return (*Random::get<Random_t::weight>(
                    std::get<std::map<std::string, float>>(v)))
            .first;
      if (std::holds_alternative<std::map<stringSpec, float>>(v)) {
        auto spec = (*Random::get<Random_t::weight>(
                         std::get<std::map<stringSpec, float>>(v)))
                        .first;
        return spec.get();
      }
      if (std::holds_alternative<std::vector<std::string>>(v)) {
        auto r = std::get<std::vector<std::string>>(v);
        return r.at(Random::get<int>(0, r.size() - 1));
      }
      throw std::runtime_error("spec wasn't matched");
    }
  }
};

template <> struct fmt::formatter<stringSpec> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  auto format(stringSpec spec, format_context &ctx) const {
    std::string res = "";
    if (!spec.value) {
      res = "<rnd>";
    } else {
      auto v = spec.value.value();
      if (std::holds_alternative<std::string>(v))
        res = fmt::format("{}", std::get<std::string>(v));
      if (std::holds_alternative<std::map<std::string, float>>(v)) {
        auto p = std::get<std::map<std::string, float>>(v);
        res += "{";
        for (auto [k, v] : p) {
          res += fmt::format("{}: {}, ", k, v);
        }
        res += "}";
      }
      if (std::holds_alternative<std::map<stringSpec, float>>(v)) {
        auto p = std::get<std::map<stringSpec, float>>(v);
        res += "{";
        for (auto [k, v] : p) {
          res += fmt::format(fmt::runtime("{}: {}, "), k, v);
        }
        res += "}";
      }
    }

    return fmt::format_to(ctx.out(), "{}", res);
  };
};
