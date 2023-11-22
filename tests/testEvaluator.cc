#include <gtest/gtest.h>

#include "logic/evaluator.h"

using namespace logic;

TEST(Evaluator, TestAssignment) {
  auto evaluator = Evaluator();

  evaluator.evaluate("");
}
