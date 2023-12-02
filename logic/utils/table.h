#pragma once

#include "logic/utils/macros.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <string>

namespace logic {

class Column {
private:
  std::vector<std::string> cells;
  size_t maxCellWidth = 0;

public:
  constexpr auto add(std::string cell) -> void {
    maxCellWidth = std::max(cell.length(), maxCellWidth);
    cells.push_back(cell);
  }
  constexpr auto numberOfCells() const -> size_t {
    return cells.size();
  }
  constexpr auto operator[](size_t i) const -> const std::string& {
    return cells.at(i);
  }
  constexpr auto getMaxWidth() const -> const size_t {
    size_t maxWidth = 0;
    for (const auto& cell : cells) {
      maxWidth = std::max(maxWidth, cell.size());
    }
    return maxWidth;
  }
};

class Table {
private:
  std::vector<Column> columns;
  size_t padding = 1;
  size_t maxColumnLength = 0;

public:
  constexpr auto numberOfColumns() const -> size_t { 
    return columns.size(); 
  }
  constexpr auto setPadding(size_t value) -> void {
    padding = value;
  }
  constexpr auto add(Column column) -> void {
    // columns must have the same number of cells
    if (maxColumnLength == 0) {
      maxColumnLength = column.numberOfCells();
    } else {
      ASSERT(maxColumnLength == column.numberOfCells());
    }
    columns.push_back(column);
  }
  auto print() const -> void;
private:
  auto printSeparationLine() const -> void;
};

};
