#include <iostream>

#include <gtest/gtest.h>

#include "logic/evaluation/evaluator.h"
#include "logic/evaluation/value.h"
#include "logic/parsing/scanner.h"
#include "logic/parsing/parser.h"
#include "logic/utils/macros.h"
#include "logic/utils/overloaded.h"
#include "tests/reporter.h"

using namespace logic;


auto verifyResult(std::string_view source, Value expectedValue) {

  auto scanner = Scanner(source);
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    FAIL() << report(tokens.error());
  }

  auto parser = Parser(std::move(*tokens));
  auto sentences = parser.parse();

  if (not sentences.has_value()) {
    FAIL() << report(sentences.error());
  }

  auto environment = Environment();
  auto evaluator = Evaluator(environment);
  auto value = evaluator.evaluate(sentences->at(0));

  if (not value.has_value()) {
    FAIL() << report(value.error());
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

