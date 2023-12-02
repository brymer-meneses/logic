#pragma once

#include "logic/evaluation/value.h"
#include "logic/parsing/sentence.h"
#include "logic/evaluation/environment.h"

#include "logic/utils/macros.h"
#include "utils/table.h"

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

  struct MaximumVariablesAchieved {
    SourceLocation location;
    explicit MaximumVariablesAchieved(SourceLocation location) : location(location) {}
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

private:
  Environment mEnvironment;
  Table table;

private:
  auto initializeEnvironment(const Sentence&) -> void;
  auto internalEvaluate(const Sentence&) const -> std::expected<Value, EvaluatorError>;

  auto printValue(const Value&) -> void;
  auto printEnvironment() -> void;

  auto negation(Value) const -> Value;
  auto conjunction(Value, Value) const -> Value;
  auto disjunction(Value, Value) const -> Value;
  auto implication(Value, Value) const -> Value;
  auto bijection(Value, Value) const -> Value;

public:
  auto evaluate(const Sentence&) -> std::expected<Value, EvaluatorError>;
};

}
