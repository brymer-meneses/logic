
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
  recordEnvironment();
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
      [this, &sentence](const Sentence::Negated& s) -> std::expected<Value, EvaluatorError> {
        const auto rhs = TRY(internalEvaluate(*s.sentence));
        const auto result = negation(rhs);
        recordSentenceEvaluation(sentence, result);
        return result;
      },
      [this, &sentence](const Sentence::Compound& s) -> std::expected<Value, EvaluatorError> {
        const auto lhs = TRY(internalEvaluate(*s.left));
        const auto rhs = TRY(internalEvaluate(*s.right));

        Value result{};
        switch (s.connective.type) {
          case TokenType::And: 
            result = conjunction(lhs, rhs);
            break;
          case TokenType::Or: 
            result = disjunction(lhs, rhs);
            break;
          case TokenType::Implies:
            result = implication(lhs, rhs);
            break;
          case TokenType::Equivalent:
            result = bijection(lhs, rhs);
            break;
          default:
            std::unreachable();
        }

        recordSentenceEvaluation(*s.left, lhs);
        recordSentenceEvaluation(*s.right, rhs);
        recordSentenceEvaluation(sentence, result);
        return result;
      },
    }
  );
}

auto Evaluator::initializeEnvironment(const Sentence& sentence) -> void {
  sentence.accept(overloaded {
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

auto Evaluator::printEvaluation() -> void {
  mTable.print();
}

auto Evaluator::recordSentenceEvaluation(const Sentence& sentence, const Value& result) const -> void {
  if (sentence.is<Sentence::Variable>()) return;
  if (sentence.is<Sentence::Value>()) return;

  auto stringRep = sentenceAsString(sentence);
  for (const auto& column : mTable.columns()) {
    if (column[0] == stringRep) { return; }
  }

  Column column;
  column.add(stringRep);
  for (const auto& boolean : result.data) {
    column.add( boolean ? "T" : "F" );
  }
  mTable.add(std::move(column));
}

auto Evaluator::recordEnvironment() const -> void {
  for (const auto& variable : mEnvironment.variables()) {
    Column column;
    column.add(std::string(variable));
    for (const auto& boolean : mEnvironment.read(variable)) {
      column.add( boolean ? "T" : "F" );
    }
    mTable.add(std::move(column));
  }
}

