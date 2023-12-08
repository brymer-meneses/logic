#include <print>

#include "logic/logic.h"
#include "logic/utils/color.h"

using namespace logic;

auto main(int argc, const char** argv) -> int {

  if (argc == 1) {
    Logic::runREPL();
  } else if (argc == 2) {
    Logic::runFile(argv[1]);
  } else {
    std::println(stderr, "Usage: logic <source>");
  }
}
