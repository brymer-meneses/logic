#include <gtest/gtest.h>

#include "logic/evaluation/environment.h"

using namespace logic;

TEST(Environment, TestVariableRead) {
  auto env = Environment();

  for (auto v : {"P", "Q", "S"}) {
    env.define(v);
  }

  EXPECT_EQ(env.totalVariablesDefined(), 3);

  auto P = std::vector<bool> {true, true, true, true, false, false, false, false};
  auto Q = std::vector<bool> {true, true, false, false, true, true, false, false};
  auto S = std::vector<bool> {true, false, true, false, true, false, true, false};

  EXPECT_EQ(env.read("P"), P);
  EXPECT_EQ(env.read("Q"), Q);
  EXPECT_EQ(env.read("S"), S);
}

TEST(Environment, TestVariableAssignment) {
  auto env = Environment();

  for (auto v : {"P", "Q", "S"}) {
    env.define(v);
  }

  env.assign("A", true);
  env.assign("B", false);

  EXPECT_EQ(env.totalVariablesDefined(), 3);
  EXPECT_EQ(env.totalVariablesAssigned(), 2);

  auto length = 1 << env.totalVariablesDefined();

  EXPECT_EQ(env.isVariableAssigned("P"), false);
  EXPECT_EQ(env.isVariableDefined("A"), false);

  EXPECT_EQ(env.read("A"), std::vector<bool>(length, true));
  EXPECT_EQ(env.read("B"), std::vector<bool>(length, false));
}
