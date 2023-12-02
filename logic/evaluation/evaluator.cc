
#include "logic/evaluation/evaluator.h"
#include "logic/evaluation/environment.h"
#include "logic/parsing/scanner.h"
#include "logic/parsing/parser.h"

#include "logic/utils/macros.h"
#include "logic/utils/overloaded.h"

#include <set>
#include <ranges>
#include <utility>

using namespace logic;

#define DEFINE_CONNECTIVE_EVALUATION(name, expr)                               \
  auto name(Value left, Value right) const->Value {                            \
    static constexpr auto operation = [](bool p, bool q) { return expr; };     \
    ASSERT(left.data.size() == right.data.size(), "Left: {} Right: {}",        \
           left.data.size(), right.data.size());                               \
    std::vector<bool> result(left.data.size());                                \
    for (int i = 0; i < left.data.size(); i++) {                               \
      result[i] = operation(left.data[i], right.data[i]);                      \
    }                                                                          \
    return result;                                                             \
  }

DEFINE_CONNECTIVE_EVALUATION(Evaluator::implication, not p or q);
DEFINE_CONNECTIVE_EVALUATION(Evaluator::bijection, p == q);
DEFINE_CONNECTIVE_EVALUATION(Evaluator::conjunction, p and q);
DEFINE_CONNECTIVE_EVALUATION(Evaluator::disjunction, p or q);

auto Evaluator::negation(Value v) const -> Value {
  std::vector<bool> result(v.data.size());
  for (int i = 0; i < v.data.size(); i++) {
    result[i] = not v.data[i];
  }
  return result;
}

auto Evaluator::evaluate(const Sentence& sentence) -> std::expected<Value, EvaluatorError> {
  initializeEnvironment(sentence);
  return internalEvaluate(sentence);
}

auto Evaluator::internalEvaluate(const Sentence& sentence) const -> std::expected<Value, EvaluatorError> {
  return sentence.accept(
    overloaded {
      [this](const Sentence::Variable& s) -> std::expected<Value, EvaluatorError> {
        return mEnvironment.read(s.identifier.lexeme);
      },
      [this](const Sentence::Value& s) -> std::expected<Value, EvaluatorError> {
        ASSERT(s.value.type == TokenType::True or s.value.type == TokenType::False);
        return mEnvironment.createBoolean(s.value.type == TokenType::True);
      },
      [this](const Sentence::Grouped& s) -> std::expected<Value, EvaluatorError> {
        return internalEvaluate(*s.sentence);
      },
      [this](const Sentence::Negated& s) -> std::expected<Value, EvaluatorError> {
        const auto rhs = TRY(internalEvaluate(*s.sentence));
        return negation(rhs);
      },
      [this](const Sentence::Compound& s) -> std::expected<Value, EvaluatorError> {
        const auto lhs = TRY(internalEvaluate(*s.left));
        const auto rhs = TRY(internalEvaluate(*s.right));

        switch (s.connective.type) {
          case TokenType::And: 
            return conjunction(lhs, rhs);
          case TokenType::Or: 
            return disjunction(lhs, rhs);
          case TokenType::Implies:
            return implication(lhs, rhs);
          case TokenType::Equivalent:
            return bijection(lhs, rhs);
          default:
            std::unreachable();
        }
      },
    }
  );
}

auto Evaluator::initializeEnvironment(const Sentence& s) -> void {
  s.accept(overloaded {
    [&](const Sentence::Variable& s){ 
      mEnvironment.define(s.identifier.lexeme);
    },
    [this](const Sentence::Grouped& s) { 
      initializeEnvironment(*s.sentence);
    },
    [this](const Sentence::Negated& s) { 
      initializeEnvironment(*s.sentence);
    },
    [this](const Sentence::Compound& s) { 
      initializeEnvironment(*s.left);
      initializeEnvironment(*s.right);
    },
    [](const Sentence::Value& s) {},
  });
}

auto Evaluator::printValue(const Value& value) -> void {
  Column column;
  for (const auto& boolean : value.data) {
    column.add( boolean ? "T" : "F" );
  }
  table.add(std::move(column));
}

