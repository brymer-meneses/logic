#include "logic/utils/table.h"
#include "logic/utils/macros.h"

#include <__format/format_functions.h>
#include <print>

using namespace logic;

auto Table::print() const -> void {
  for (auto j = 0; j < mMaxColumnLength; j++) {
    if (j == 0 or j == 1) {
      printSeparationLine();
    }

    for (auto i = 0; i < mColumns.size(); i++) {
      // print the contents
      auto formatStr = std::format("|{{: ^{}}}", mColumns[i].getMaxWidth() + 2*mPadding);
      auto str = std::vformat(formatStr, std::make_format_args(mColumns[i][j]));
      std::print("{}", str);

      // print the bar and the endline
      if (i == mColumns.size() - 1) {
        std::println("|");
      }
    }
  }
  printSeparationLine();
}

auto Table::printSeparationLine() const -> void {
  for (auto i = 0; i < mColumns.size(); i++) {
    auto length = mColumns[i].getMaxWidth() + 2*mPadding;
    auto formatStr = std::format("+{{:-^{}}}", length);
    auto str = std::vformat(formatStr, std::make_format_args(""));
    std::print("{}", str);
  }
  std::println("+");
}
