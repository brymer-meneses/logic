#pragma once

#include <cstddef>
#include <string_view>

namespace logic {

struct SourceLocation {
  size_t start;
  size_t end;
  size_t line;
  std::string_view filename;

  constexpr SourceLocation(size_t start, size_t end, size_t line, std::string_view filename)
    : start(start), end(end), line(line), filename(filename) {}

  friend constexpr auto operator==(const SourceLocation&, const SourceLocation&) -> bool = default;
};

}
