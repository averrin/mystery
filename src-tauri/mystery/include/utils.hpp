#pragma once

#include <bits/ranges_algo.h>
#include <functional>
#include <map>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
namespace views = std::ranges::views;
// namespace ranges = std::ranges;
#include "fmt/core.h"
#include "fmt/format.h"

struct to_vector_adapter {
  struct closure {
    template <std::ranges::range R> constexpr auto operator()(R &&r) const {
      std::vector<std::ranges::range_value_t<R>> v;
      for (auto i : r) {
        v.push_back(i);
      }
      return v;
      // auto r_common = r | std::views::common;

      // if constexpr (requires { std::ranges::size(r); }) {
      //   v.reserve(std::ranges::size(r));
      // }

      // v.insert(v.begin(), r_common.begin(), r_common.end());

      // return v;
    }
  };
  constexpr auto operator()() const -> closure { return closure{}; }

  template <std::ranges::range R> constexpr auto operator()(R &&r) {
    return closure{}(r);
  }
};

inline to_vector_adapter to_vector;
template <std::ranges::range R>
constexpr auto operator|(R &&r, to_vector_adapter::closure const &a) {
  return a(std::forward<R>(r));
}

struct unique_adapter {
  struct closure {
    template <std::ranges::range R> constexpr auto operator()(R &&r) const {
      auto v = r | to_vector();
      std::ranges::sort(v, std::ranges::greater());
      auto _r = std::ranges::unique(v);
      v.erase(_r.begin(), _r.end());

      return v;
    }
  };
  constexpr auto operator()() const -> closure { return closure{}; }

  template <std::ranges::range R> constexpr auto operator()(R &&r) {
    return closure{}(r);
  }
};

inline unique_adapter unique;
template <std::ranges::range R>
constexpr auto operator|(R &&r, unique_adapter::closure const &a) {
  return a(std::forward<R>(r));
}
