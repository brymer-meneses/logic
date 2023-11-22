#include "logic/value.h"
#include "logic/utils.h"

using namespace logic;

auto Value::logicalNot() -> Value {
  return accept(overloaded {
    [](bool value) {
      return Value(not value);
    },
    [](std::array<bool, 4> value) {
      std::array<bool, 4> newValue;
      for (int i=0; i<4; i++) {
        newValue[i] = not value[i];
      }
      return Value(newValue);
    }
  });
}

auto Value::logicalImplies(Value other) -> Value {

  constexpr auto implies = [](bool P, bool Q){
    return not P or Q;
  };

  if (this->isScalar() and other.isScalar()) {
    return implies(std::get<bool>(value), std::get<bool>(other.value));
  }

  if (this->isScalar() and not other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = implies(std::get<bool>(value), std::get<1>(other.value)[i]);
    }
    return newValue;
  }

  if (not this->isScalar() and other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = implies(std::get<bool>(other.value), std::get<1>(value)[i]);
    }
    return newValue;
  }

  if (not this->isScalar() and not other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = implies(std::get<1>(other.value)[i], std::get<1>(value)[i]);
    }
    return newValue;
  }

  __builtin_unreachable();

}

auto Value::logicalOr(Value other) -> Value {
  if (this->isScalar() and other.isScalar()) {
    return std::get<bool>(value) or std::get<bool>(other.value);
  }

  if (this->isScalar() and not other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = std::get<bool>(value) or std::get<1>(other.value)[i];
    }
    return newValue;
  }

  if (not this->isScalar() and other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = std::get<bool>(other.value) or std::get<1>(value)[i];
    }
    return newValue;
  }

  if (not this->isScalar() and not other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = std::get<1>(other.value)[i] or std::get<1>(value)[i];
    }
    return newValue;
  }

  __builtin_unreachable();
}

auto Value::logicalAnd(Value other) -> Value {

  if (this->isScalar() and other.isScalar()) {
    return std::get<bool>(value) and std::get<bool>(other.value);
  }

  if (this->isScalar() and not other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = std::get<bool>(value) and std::get<1>(other.value)[i];
    }
    return newValue;
  }
  if (not this->isScalar() and other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = std::get<bool>(other.value) and std::get<1>(value)[i];
    }
    return newValue;
  }

  if (not this->isScalar() and not other.isScalar()) {
    std::array<bool, 4> newValue;
    for (int i = 0; i<4; i++) {
      newValue[i] = std::get<1>(other.value)[i] and std::get<1>(value)[i];
    }
    return newValue;
  }

  __builtin_unreachable();
}

