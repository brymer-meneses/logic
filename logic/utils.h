#pragma once

#include <print>
#include <source_location>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

#define RAISE_INTERNAL_ERROR(...) \
{ \
  const auto location = std::source_location::current(); \
  std::println(stderr, "[{}:{}]: {}", location.file_name(), location.line(), location.function_name()); \
  std::println(stderr, "INTERNAL ERROR: " __VA_ARGS__); \
  exit(1); \
} \
