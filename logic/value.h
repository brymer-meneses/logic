#pragma once

#include <array>
#include <variant>

namespace logic {

class Value {

private:
  std::variant<bool, std::array<bool, 4>> value;

public:
  constexpr auto accept(auto visitor) -> decltype(auto) {
    return std::visit(visitor, value);
  }

  constexpr Value(bool value) : value(value) {}
  constexpr Value(std::array<bool, 4> value) : value(value) {}

  auto logicalNot() -> Value;
  auto logicalAnd(Value) -> Value;
  auto logicalOr(Value) -> Value;
  auto logicalImplies(Value) -> Value;

  constexpr auto isScalar() const -> bool {
    return std::holds_alternative<bool>(value);
  }

};

}
