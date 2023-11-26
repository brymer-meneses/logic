#pragma once

#include "logic/sentence.h"
#include "logic/utils.h"
#include "logic/value.h"
#include "logic/environment.h"

#include <string_view>
#include <vector>
#include <cstddef>
#include <expected>

namespace logic {

class EvaluatorError {

public:
  struct InvalidVariableName {
    Token variable;

    explicit InvalidVariableName(Token variable) : variable(variable) {}
  };

private:
  using ValueType = std::variant<InvalidVariableName>;
  ValueType value;

public:
  template <typename T>
  requires std::is_constructible_v<ValueType, T>
  constexpr EvaluatorError(T value) : value(value) {}

  constexpr auto accept(auto visitor) -> decltype(auto) {
    return std::visit(visitor, value);
  } 
};

class Evaluator {
  Environment mEnvironment;

private:
  auto initializeEnvironment(const Sentence&) -> void;
  auto internalEvaluate(const Sentence&) const -> std::expected<Value, EvaluatorError>;

public:
  auto evaluate(const Sentence&) -> std::expected<Value, EvaluatorError>;

  auto negation(Value) const -> Value;
  auto conjunction(Value, Value) const -> Value;
  auto disjunction(Value, Value) const -> Value;
  auto implication(Value, Value) const -> Value;
  auto bijection(Value, Value) const -> Value;
};

}
