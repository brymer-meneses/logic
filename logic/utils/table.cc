#include "logic/utils/table.h"
#include "logic/utils/macros.h"
#include "logic/utils/color.h"

using namespace logic;

auto Table::print() const -> void {
  static auto line = Color::Gray("|");
  for (auto j = 0; j < mMaxColumnLength; j++) {
    if (j == 0 or j == 1) {
      printSeparationLine();
    }

    for (auto i = 0; i < mColumns.size(); i++) {
      // print the contents
      auto formatStr = fmt::format("{{: ^{}}}", mColumns[i].getMaxWidth() + 2*mPadding);
      auto str = fmt::vformat(formatStr, fmt::make_format_args(mColumns[i][j]));
      fmt::print("{}{}", line, str);

      // print the bar and the endline
      if (i == mColumns.size() - 1) {
        fmt::println("{}", line);
      }
    }
  }
  printSeparationLine();
}

auto Table::printSeparationLine() const -> void {
  for (auto i = 0; i < mColumns.size(); i++) {
    auto length = mColumns[i].getMaxWidth() + 2*mPadding;
    auto formatStr = fmt::format("+{{:-^{}}}", length);
    auto str = fmt::vformat(formatStr, fmt::make_format_args(""));
    fmt::print("{}", Color::Gray(str));
  }

  if (mColumns.size() != 0) {
    fmt::println("{}", Color::Gray("+"));
  }
}
