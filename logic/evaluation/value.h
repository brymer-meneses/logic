#pragma once

#include <variant>
#include <vector>

#include "logic/utils/macros.h"

namespace logic {

class Value {

public:
  std::vector<bool> data;

  constexpr Value(bool value) : data{value} {}
  constexpr Value(std::vector<bool> value) : data(value) {}
  constexpr Value(std::initializer_list<bool> value) : data(value) {}

  constexpr Value(bool value, size_t size) : data(size) {
    for (size_t i=0; i<size; i++) {
      data[i] = value;
    }
  }

  friend constexpr auto operator==(const Value&, const Value&) -> bool = default;
};

}
