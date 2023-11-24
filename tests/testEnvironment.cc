#include <gtest/gtest.h>

#include "logic/environment.h"

using namespace logic;

TEST(Environment, TestVariableRead) {
  auto env = Environment(3);
  env.defineVariable("P");
  env.defineVariable("Q");
  env.defineVariable("S");
  //
  auto P = std::vector<bool> {true, true, true, true, false, false, false, false};
  auto Q = std::vector<bool> {true, true, false, false, true, true, false, false};
  auto S = std::vector<bool> {true, false, true, false, true, false, true, false};

  EXPECT_EQ(env.readVariable("P"), P);
  EXPECT_EQ(env.readVariable("Q"), Q);
  EXPECT_EQ(env.readVariable("S"), S);
}
