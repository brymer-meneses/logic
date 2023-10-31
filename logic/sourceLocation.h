#pragma once

#include <cstddef>

namespace logic {

struct SourceLocation {
  size_t start;
  size_t end;
  size_t line;

  constexpr SourceLocation(size_t start, size_t end, size_t line)
    : start(start), end(end), line(line) {}
};

}
