#include "logic/utils/table.h"
#include "logic/utils/macros.h"

#include <__format/format_functions.h>
#include <print>

using namespace logic;

auto Table::print() const -> void {
  for (auto j = 0; j < maxColumnLength; j++) {
    printSeparationLine();
    for (auto i = 0; i < columns.size(); i++) {
      // print the contents
      auto formatStr = std::format("|{{: ^{}}}", columns[i].getMaxWidth() + 2*padding);
      auto str = std::vformat(formatStr, std::make_format_args(columns[i][j]));
      std::print("{}", str);

      // print the bar and the endline
      if (i == columns.size() - 1) {
        std::println("|");
      }
    }
  }
  printSeparationLine();
}

auto Table::printSeparationLine() const -> void {
  for (auto i = 0; i < columns.size(); i++) {
    auto length = columns[i].getMaxWidth() + 2*padding;
    auto formatStr = std::format("+{{:-^{}}}", length);
    auto str = std::vformat(formatStr, std::make_format_args(""));
    std::print("{}", str);
  }
  std::println("+");
}
