#pragma once

#include "utils.h"
#include <array>
#include <variant>
#include <iostream>
#include <vector>

namespace logic {

class Value {

private:
  std::vector<bool> data;

public:

  constexpr Value(bool value) : data{value} {}
  constexpr Value(std::vector<bool> value) : data(value) {}
  constexpr Value(std::initializer_list<bool> value) : data(value) {}

  constexpr Value(bool value, size_t size) : data(size) {
    for (size_t i=0; i<size; i++) {
      data[i] = value;
    }
  }

  friend constexpr auto operator==(const Value&, const Value&) -> bool = default;
  friend constexpr auto operator<<(std::ostream& stream, const Value& line) -> std::ostream&;

  friend class Evaluator;
};

}
