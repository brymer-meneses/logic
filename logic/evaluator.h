#pragma once

#include "logic/sentence.h"
#include "logic/utils.h"
#include "logic/value.h"

#include <string_view>
#include <cstddef>
#include <optional>
#include <expected>

namespace logic {

class EvaluatorError {

};

class Environment {
  std::optional<Value> P;
  std::optional<Value> Q;
  std::optional<Value> S;

  uint8_t usedVariables = 0;
};

class Evaluator {

private:
  Environment environment;

public:
  auto evaluate(std::string_view) -> void;
  auto evaluateSentence(const Sentence&) -> std::expected<Value, EvaluatorError>;

private:
  auto defineVariable(Token) -> void;

};

}
