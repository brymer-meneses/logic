#include <iostream>
#include <format>

#include <gtest/gtest.h>

#include "logic/evaluator.h"
#include "logic/scanner.h"
#include "logic/parser.h"
#include "logic/value.h"
#include "logic/utils.h"

using namespace logic;

constexpr auto logic::operator<<(std::ostream& stream, const Value& value) -> std::ostream& {
  return stream << std::format("{})", value.data);
}

auto verifyResult(std::string_view source, Value expectedValue) {

  auto scanner = Scanner(source);
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    FAIL() << tokens.error().accept(overloaded {
        [](const ScannerError::UnexpectedKeyword &e) {
          return std::format("Unexpected keyword `{}`", e.keyword);
        },
        [](const ScannerError::UnexpectedCharacter &e) {
          return std::format("Unexpected character `{}`", e.character);
        }}
      );
  }

  auto parser = Parser(std::move(*tokens));
  auto sentences = parser.parse();

  if (not sentences.has_value()) {
    FAIL() << sentences.error().accept(overloaded {
      [](const ParserError::ExpectedToken& e) {
        return std::format("Unexpected token {}, expected {}", e.got.lexeme, tokenTypeToString(e.expected));
      }
    });
  }

  auto evaluator = Evaluator();
  auto value = evaluator.evaluate(sentences->at(0));

  if (not value.has_value()) {
    FAIL();
  }

  EXPECT_EQ(*value, expectedValue);
}

TEST(Evaluator, TestScalar) {
  verifyResult("TRUE", true);
  verifyResult("FALSE", false);

  verifyResult("NOT TRUE", false);
  verifyResult("NOT FALSE", true);

  verifyResult("NOT NOT TRUE", true);
  verifyResult("NOT NOT FALSE", false);

  verifyResult("TRUE EQUIVALENT TRUE", true);
  verifyResult("TRUE EQUIVALENT FALSE", false);
  verifyResult("FALSE EQUIVALENT FALSE", true);

  verifyResult("TRUE OR TRUE", true);
  verifyResult("TRUE OR FALSE", true);

  verifyResult("TRUE AND TRUE", true);
  verifyResult("TRUE AND FALSE", false);

  verifyResult("TRUE IMPLIES TRUE", true);
  verifyResult("TRUE IMPLIES FALSE", false);
}

TEST(Evaluator, TestSingleVariable) {
  verifyResult("P", {true, false});
  verifyResult("NOT P", {false, true});

  verifyResult("P EQUIVALENT TRUE", {true, false});
  verifyResult("P EQUIVALENT FALSE", {false, true});
  
  verifyResult("P OR TRUE", {true, true});
  verifyResult("P OR FALSE", {true, false});
  
  verifyResult("P AND TRUE", {true, false});
  verifyResult("P AND FALSE", {false, false});
  
  verifyResult("P IMPLIES TRUE", {true, true});
  verifyResult("P IMPLIES FALSE", {false, true});
}

TEST(Evaluator, TestMultipleVariables) {
  verifyResult("P EQUIVALENT Q", {true, false, false, true});
  verifyResult("P OR Q", {true, true, true, false});
  verifyResult("P AND Q", {true, false, false, false});
  verifyResult("P IMPLIES Q", {true, false, true, true});

  verifyResult("(P EQUIVALENT Q) EQUIVALENT S", {true, false, false, true, false, true, true, false});
  verifyResult("(P OR Q) OR S", {true, true, true, true, true, true, true, false});
  verifyResult("(P AND Q) AND S", {true, false, false, false, false, false, false, false});
  verifyResult("(P IMPLIES Q) IMPLIES S", {true, false, true, true, true, false, true ,false});
}

