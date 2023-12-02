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
  std::vector<Column> mColumns;
  size_t mPadding = 1;
  size_t mMaxColumnLength = 0;

public:
  constexpr auto numberOfColumns() const -> size_t { 
    return mColumns.size(); 
  }
  constexpr auto columns() const -> const std::vector<Column>& { 
    return mColumns; 
  }
  constexpr auto setPadding(size_t value) -> void {
    mPadding = value;
  }
  constexpr auto add(Column column) -> void {
    // columns must have the same number of cells
    if (mMaxColumnLength == 0) {
      mMaxColumnLength = column.numberOfCells();
    } else {
      ASSERT(mMaxColumnLength == column.numberOfCells());
    }
    mColumns.push_back(column);
  }
  auto print() const -> void;
private:
  auto printSeparationLine() const -> void;
};

};
