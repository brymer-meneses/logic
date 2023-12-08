#include <gtest/gtest.h>

auto main(int argc, char** args) -> int {
  testing::InitGoogleTest(&argc, args);
  return RUN_ALL_TESTS();
}
