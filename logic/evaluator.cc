
#include "logic/evaluator.h"
#include "logic/utils.h"

#include "logic/scanner.h"
#include "logic/parser.h"

#include <print>
#include <vector>

using namespace logic;

auto Evaluator::evaluate(std::string_view source) -> void {

  auto scanner = Scanner(source);
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    auto message = tokens.error().accept(overloaded {
        [](const ScannerError::UnexpectedKeyword &e) {
          return std::format("Unexpected keyword `{}`", e.keyword);
        },
        [](const ScannerError::UnexpectedCharacter &e) {
          return std::format("Unexpected character `{}`", e.character);
        }}
      );

    std::println(stderr, "{}", message);
    exit(1);
  }

  auto parser = Parser(std::move(*tokens));
  auto sentences = parser.parse();

  if (not sentences.has_value()) {
    auto message = sentences.error().accept(overloaded {
      [](const ParserError::ExpectedToken& e) {
        return std::format("Unexpected token {}, expected {}", e.got.lexeme, tokenTypeToString(e.expected));
      }
    });
    std::println(stderr, "{}", message);
    exit(1);
  }

  for (const auto& sentence : *sentences) {
    evaluateSentence(sentence);
  }
};


auto Evaluator::evaluateSentence(const Sentence& sentence) -> std::expected<Value, EvaluatorError> {
  return sentence.accept(
    overloaded {
      [](const Sentence::Variable& s) -> std::expected<Value, EvaluatorError> {
        // TOOO: lookup from the environment
        //       if it does not exist present the default
        return Value(false);
      },
      [](const Sentence::Value& s) -> std::expected<Value, EvaluatorError> {
        if (s.value.type == TokenType::True) {
          return Value(true);
        } else {
          return Value(false);
        }
      },
      [this](const Sentence::Negated& s) -> std::expected<Value, EvaluatorError> {
        auto rhs = TRY(evaluateSentence(*s.sentence));
        return rhs.logicalNot();
      },
      [this](const Sentence::Compound& s) -> std::expected<Value, EvaluatorError> {
        switch (s.connective.type) {
          case TokenType::And: {
            auto lhs = TRY(evaluateSentence(*s.left));
            auto rhs = TRY(evaluateSentence(*s.left));
            return lhs.logicalAnd(rhs);
          }
          case TokenType::Or: {
            auto lhs = TRY(evaluateSentence(*s.left));
            auto rhs = TRY(evaluateSentence(*s.left));
            return lhs.logicalAnd(rhs);
          }

          case TokenType::Implies: {
            auto lhs = TRY(evaluateSentence(*s.left));
            auto rhs = TRY(evaluateSentence(*s.left));
            return lhs.logicalImplies(rhs);
          }

          default:
            __builtin_unreachable();
        }
      },
      [](const auto& s1) -> std::expected<Value, EvaluatorError> {
        return Value(false);
      }
    }
  );
}

