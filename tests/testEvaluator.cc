#include <gtest/gtest.h>

#include "logic/evaluator.h"
#include "logic/scanner.h"
#include "logic/parser.h"

using namespace logic;

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

  EXPECT_EQ(value, expectedValue);
}

TEST(Evaluator, TestScalar) {
  verifyResult("TRUE", Value(true));
  verifyResult("FALSE", Value(false));

  verifyResult("NOT TRUE", Value(false));
  verifyResult("NOT FALSE", Value(true));

  verifyResult("NOT NOT TRUE", Value(true));
  verifyResult("NOT NOT FALSE", Value(false));

  verifyResult("TRUE EQUIVALENT TRUE", Value(true));
  verifyResult("TRUE EQUIVALENT FALSE", Value(false));

  verifyResult("TRUE OR TRUE", Value(true));
  verifyResult("TRUE OR FALSE", Value(true));

  verifyResult("TRUE AND TRUE", Value(true));
  verifyResult("TRUE AND FALSE", Value(false));

  verifyResult("TRUE IMPLIES TRUE", Value(true));
  verifyResult("TRUE IMPLIES FALSE", Value(false));
}

TEST(Evaluator, TestSingleVariable) {
  verifyResult("P", Value({true, false}));
  // verifyResult("NOT P", Value({false, true}));

  //
  // verifyResult("P EQUIVALENT TRUE", Value({true, true, false, false}));
  // verifyResult("P EQUIVALENT FALSE", Value({false, false, true, true}));
  //
  // verifyResult("P OR TRUE", Value({true, true, true, true}));
  // verifyResult("P OR FALSE", Value({true, true, false, false}));
  //
  // verifyResult("P AND TRUE", Value({true, true, false, false}));
  // verifyResult("P AND FALSE", Value({false, false, false, false}));
  //
  // verifyResult("P IMPLIES TRUE", Value({true, true, true, true}));
  // verifyResult("P IMPLIES FALSE", Value({false, false, true, true}));
}

// TEST(Evaluator, TestMultipleVariables) {
//   verifyResult("P", Value({true, true, false, false}));
//   verifyResult("NOT P", Value({false, false, true, true}));
//
//   verifyResult("Q", Value({true, false, true, false}));
//   verifyResult("NOT Q", Value({false, true, false, true}));
//
//   verifyResult("P EQUIVALENT Q", Value({true, false, false, true}));
//   verifyResult("P OR Q", Value({true, true, true, false}));
//   verifyResult("P AND Q", Value({true, false, false, false}));
//   verifyResult("P IMPLIES Q", Value({true, false, true, true}));
// }
