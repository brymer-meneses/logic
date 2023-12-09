#pragma once

#include "logic/evaluation/value.h"
#include "logic/parsing/sentence.h"
#include "logic/evaluation/environment.h"

#include "logic/utils/macros.h"
#include "logic/utils/table.h"

#include <string_view>
#include <vector>
#include <cstddef>
#include <expected>

namespace logic {

class EvaluatorError {

public:
  struct MaximumVariablesAchieved {
    SourceLocation location;
    explicit MaximumVariablesAchieved(SourceLocation location) : location(location) {}
  };

  struct InvalidAssignment {
    SourceLocation location;
    explicit InvalidAssignment(SourceLocation location) : location(location) {}
  };

private:
  using ValueType = std::variant<MaximumVariablesAchieved, InvalidAssignment>;
  ValueType value;

public:
  template <typename T>
  requires std::is_constructible_v<ValueType, T>
  constexpr EvaluatorError(T value) : value(value) {}

  constexpr auto accept(auto visitor) const -> decltype(auto) {
    return std::visit(visitor, value);
  } 
};

class Evaluator {

private:
  Environment mEnvironment;
  mutable Table mTable;

private:
  auto initializeEnvironment(const Sentence&) -> void;
  auto internalEvaluate(const Sentence&) const -> std::expected<Value, EvaluatorError>;

  auto recordSentenceEvaluation(const Sentence&, const Value&) const -> void;
  auto recordEnvironment() const -> void;

  auto negation(Value) const -> Value;
  auto conjunction(Value, Value) const -> Value;
  auto disjunction(Value, Value) const -> Value;
  auto implication(Value, Value) const -> Value;
  auto bijection(Value, Value) const -> Value;

public:
  auto evaluate(const Sentence&) -> std::expected<Value, EvaluatorError>;

  auto printEvaluation() -> void;
};

}
