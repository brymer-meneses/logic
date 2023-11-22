#pragma once

#include "logic/sentence.h"
#include "logic/utils.h"

#include <array>
#include <variant>

namespace logic {

class EvaluatorError {

};

class Value {

private:
  std::variant<bool, std::array<bool, 4>> value;

public:
  constexpr auto accept(auto visitor) -> decltype(auto) {
    return std::visit(visitor, value);
  }

  constexpr Value(bool value) : value(value) {}
  constexpr Value(std::array<bool, 4> value) : value(value) {}

  constexpr auto operator not() -> Value {
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
  };

  constexpr auto operator and(Value) -> Value {
    return Value(false);
  }

  constexpr auto operator or(Value) -> Value {
    return Value(false);
  }

};

class Environment {
  Value P;
  Value Q;
  Value S;
};

class Evaluator {

private:
  Environment environment;

public:
  auto evaluate(const Sentence&) -> Value;
  auto evaluate(std::vector<Sentence>) -> Value;
};

}
