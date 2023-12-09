#pragma once

#include <cstddef>
#include <string_view>

#include <algorithm>

namespace logic {

struct SourceLocation {
  size_t start;
  size_t end;
  size_t line;
  std::string_view filename;

  constexpr SourceLocation(size_t start, size_t end, size_t line, std::string_view filename)
    : start(start), end(end), line(line), filename(filename) {}

  friend constexpr auto operator+(const SourceLocation& s1, const SourceLocation& s2) -> SourceLocation {
    return SourceLocation(std::min(s1.start, s2.start), std::max(s1.end, s2.end), s1.line, s1.filename);
  }
  friend constexpr auto operator==(const SourceLocation&, const SourceLocation&) -> bool = default;
};

}
