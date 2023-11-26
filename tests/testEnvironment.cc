#include <gtest/gtest.h>

#include "logic/environment.h"

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
