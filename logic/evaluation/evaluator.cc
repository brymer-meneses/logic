
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
  TRY(initializeVariables(sentence));
  recordEnvironment();
  return internalEvaluate(sentence);
}

auto Evaluator::internalEvaluate(const Sentence& sentence) const -> std::expected<Value, EvaluatorError> {
  using Result = std::expected<Value, EvaluatorError>;
  return sentence.accept(
    overloaded {
      [this, &sentence](const Sentence::Variable& s) -> Result {
        auto value = mEnvironment.read(s.identifier.lexeme); 
        recordEvaluation(sentence, value);
        return value;
      },
      [this](const Sentence::Value& s) -> Result {
        ASSERT(s.value.type == TokenType::True or s.value.type == TokenType::False);
        return mEnvironment.createBoolean(s.value.type == TokenType::True);
      },
      [this](const Sentence::Grouped& s) -> Result {
        return internalEvaluate(*s.sentence);
      },
      [this, &sentence](const Sentence::Negated& s) -> Result {
        const auto rhs = TRY(internalEvaluate(*s.sentence));
        const auto result = negation(rhs);
        recordEvaluation(sentence, result);
        return result;
      },
      [this, &sentence](const Sentence::Compound& s) -> Result {
        if (s.connective.type == TokenType::Equal) { 
          // NOTE:
          //    we can already be sure that this is a valid assignment since that is checked 
          //    before-hand by 'recordEvaluation'
          return internalEvaluate(*s.right);
        }

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

        recordEvaluation(*s.left, lhs);
        recordEvaluation(*s.right, rhs);
        recordEvaluation(sentence, result);
        return result;
      },
    }
  );
}

auto Evaluator::initializeVariables(const Sentence& sentence) -> std::expected<bool, EvaluatorError>{
  return sentence.accept(overloaded {
    [&](const Sentence::Variable& s) -> std::expected<bool, EvaluatorError> { 
      if (not mEnvironment.isVariableAssigned(s.identifier.lexeme)) {
        mEnvironment.define(s.identifier.lexeme);
      }
      return true;
    },
    [this](const Sentence::Grouped& s) -> std::expected<bool, EvaluatorError> { 
      initializeVariables(*s.sentence);
      return true;
    },
    [this](const Sentence::Negated& s) -> std::expected<bool, EvaluatorError> {
      initializeVariables(*s.sentence);
      return true;
    },
    [this, &sentence](const Sentence::Compound& s) -> std::expected<bool, EvaluatorError> {  
      if (s.connective.type == TokenType::Equal) { 
        const auto isValidAssignment = s.left->is<Sentence::Variable>() and s.right->is<Sentence::Value>();
        if (not isValidAssignment) {
          return std::unexpected(EvaluatorError::InvalidAssignment(sentence.location()));
        }

        auto variableName = s.left->unsafeAs<Sentence::Variable>().identifier.lexeme;
        auto value = s.right->unsafeAs<Sentence::Value>().value.type == TokenType::True;
        mEnvironment.assign(variableName, value);
        return true;
      }

      initializeVariables(*s.left);
      initializeVariables(*s.right);
      return true;
    },
    [](const Sentence::Value& s) -> std::expected<bool, EvaluatorError> {  
      return true;
    },
  });
}

auto Evaluator::printEvaluation() -> void {
  mTable.print();
}

auto Evaluator::recordEvaluation(const Sentence& sentence, const Value& result) const -> void {
  if (sentence.is<Sentence::Variable>()) {
    // skip recording variables if they have "default" values, as this will be recorded by `recordEnvironment`.
    const auto isVariableAssigned = mEnvironment.isVariableAssigned(sentence.unsafeAs<Sentence::Variable>().identifier.lexeme);
    if (not isVariableAssigned) return; 
  }

  if (sentence.is<Sentence::Value>()) return;
  if (sentence.is<Sentence::Grouped>()) return;

  auto stringRep = Sentence::asString(sentence);
  for (const auto& column : mTable.columns()) {
    if (column[0] == stringRep) { return; }
  }

  Column column(result.data.size());
  column.add(stringRep);
  for (const auto& boolean : result.data) {
    column.add( boolean ? "T" : "F" );
  }
  mTable.add(std::move(column));
}

auto Evaluator::recordEnvironment() const -> void {
  for (const auto& variable : mEnvironment.definedVariables()) {
    Column column(variable.size() + 1);
    column.add(variable);
    for (const auto& boolean : mEnvironment.read(variable)) {
      column.add( boolean ? "T" : "F" );
    }
    mTable.add(std::move(column));
  }
}

