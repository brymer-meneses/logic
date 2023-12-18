#pragma once

#include <source_location>
#include <fmt/core.h>


#define RAISE_INTERNAL_ERROR(...) \
{ \
  const auto location = std::source_location::current(); \
  fmt::println(stderr, "[{}:{}]: {}", location.file_name(), location.line(), location.function_name()); \
  fmt::println(stderr, "INTERNAL ERROR: " __VA_ARGS__); \
  exit(1); \
}

#define TRY(expr)                                                              \
  ({                                                                           \
    auto temp = expr;                                                          \
    if (not temp.has_value()) {                                                \
      return std::unexpected(temp.error());                                    \
    };                                                                         \
    std::move(*temp);                                                          \
  });

#define ASSERT(expr, ...)                                                      \
  {                                                                            \
    if (not(expr)) [[unlikely]] {                                              \
      const auto location = std::source_location::current();                   \
      fmt::println(stderr, "[{}:{}]: {}", location.file_name(),                \
                   location.line(), location.function_name());                 \
      fmt::println(stderr, "ASSERTION ERROR: " __VA_ARGS__);                   \
      exit(1);                                                                 \
    }                                                                          \
  }
