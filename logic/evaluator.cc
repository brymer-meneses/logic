
#include "logic/evaluator.h"
#include "logic/utils.h"

#include "logic/scanner.h"
#include "logic/parser.h"

#include <set>
#include <utility>

using namespace logic;

using Array = std::array<bool, 4>;

#define DEFINE_CONNECTIVE_EVALUATION(name, operation)                     \
  auto name(Value left, Value right) const -> Value {                     \
    return std::visit(overloaded {                                        \
      [](const bool v1, const bool v2) -> Value {                         \
        return operation(v1, v2);                                         \
      },                                                                  \
      [](const Array& v1, const bool v2) -> Value {                       \
        Array result;                                                     \
        for (int i=0; i<v1.size(); i++) {                                 \
          result[i] = operation(v1[i], v2);                               \
        }                                                                 \
        return result;                                                    \
      },                                                                  \
      [](const bool v1, const Array& v2) -> Value {                       \
        Array result;                                                     \
        for (int i=0; i<v2.size(); i++) {                                 \
          result[i] = operation(v2[i], v1);                               \
        }                                                                 \
        return result;                                                    \
      },                                                                  \
      [](const Array& v1, const Array& v2) -> Value {                     \
        Array result;                                                     \
        for (int i=0; i <v1.size(); i++) {                                \
          result[i] = operation(v1[i], v2[i]);                            \
        }                                                                 \
        return result;                                                    \
      }                                                                   \
    }, left.data, right.data);                                            \
  }                                                                       \

static constexpr auto _implication(const bool p, const bool q) -> bool {
  return not p or q;
};

static constexpr auto _conjunction(const bool p, const bool q) -> bool {
  return p and q;
};

static constexpr auto _disjunction(const bool p, const bool q) -> bool {
  return p or q;
};

static constexpr auto _bijection(const bool p, const bool q) -> bool {
  return p == q;
};

DEFINE_CONNECTIVE_EVALUATION(Evaluator::implication, _implication);
DEFINE_CONNECTIVE_EVALUATION(Evaluator::bijection, _bijection);
DEFINE_CONNECTIVE_EVALUATION(Evaluator::disjunction, _disjunction);
DEFINE_CONNECTIVE_EVALUATION(Evaluator::conjunction, _conjunction);

auto Evaluator::evaluate(const Sentence& sentence) const -> std::expected<Value, EvaluatorError> {

  const auto numVariables = countUniqueVariables(sentence);

  // TODO:
  // if (numVariables > 2) {
  // }

  return sentence.accept(
    overloaded {
      [&numVariables](const Sentence::Variable& s) -> std::expected<Value, EvaluatorError> {
        if (s.identifier.lexeme == "P") {
          return numVariables == 1 ? Value({true, false}) : Value({true, true, false, false});
        } else if (s.identifier.lexeme == "Q") {
          return numVariables == 1 ? Value({true, false}) : Value({false, false, true, true});
        }

        return std::unexpected(EvaluatorError::InvalidVariableName(s.identifier));
      },
      [](const Sentence::Value& s) -> std::expected<Value, EvaluatorError> {
        if (s.value.type == TokenType::True) {
          return Value(true);
        } else {
          return Value(false);
        }
      },
      [this](const Sentence::Grouped& s) -> std::expected<Value, EvaluatorError> {
        return evaluate(*s.sentence);
      },
      [this](const Sentence::Negated& s) -> std::expected<Value, EvaluatorError> {
        const auto rhs = TRY(evaluate(*s.sentence));
        return negation(rhs);
      },
      [this](const Sentence::Compound& s) -> std::expected<Value, EvaluatorError> {
        const auto lhs = TRY(evaluate(*s.left));
        const auto rhs = TRY(evaluate(*s.right));

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

auto Evaluator::negation(Value v) const -> Value {
  return v.accept(overloaded {
    [](bool value) -> Value {
      return not value;
    },
    [](Array value) -> Value {
      Array newValue;
      for (int i=0; i<value.size(); i++) {
        newValue[i] = not value[i];
      }
      return newValue;
    },
  });
}

auto Evaluator::countUniqueVariables(const Sentence& s) const -> size_t {
  std::set<std::string_view> visitedVariables;
  auto numVariables = size_t{0};
  s.accept(overloaded {
    [&](const Sentence::Variable& s){ 
      if (not visitedVariables.contains(s.identifier.lexeme)) {
        numVariables += 1;
      };
    },
    [](const auto& s){}
  });
  return numVariables;
}
