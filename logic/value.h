#pragma once

#include <array>
#include <variant>

namespace logic {

class Value {

private:
  std::variant<bool, std::array<bool, 4>> data;

public:
  constexpr auto accept(auto visitor) -> decltype(auto) {
    return std::visit(visitor, data);
  }

  constexpr Value(bool value) : data(value) {}
  constexpr Value(std::array<bool, 4> value) : data(value) {}

  friend constexpr auto operator==(const Value&, const Value&) -> bool = default;
  friend class Evaluator;
};

}
