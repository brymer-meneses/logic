#pragma once

#include <array>
#include <variant>
#include <vector>

namespace logic {

class Value {

private:
  std::variant<bool, std::vector<bool>> data;

public:
  constexpr auto accept(auto visitor) -> decltype(auto) {
    return std::visit(visitor, data);
  }

  constexpr Value(bool value) : data(value) {}
  constexpr Value(std::vector<bool> value) : data(value) {}

  friend constexpr auto operator==(const Value&, const Value&) -> bool = default;
  friend class Evaluator;
};

}
