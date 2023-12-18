#pragma once

#include <fmt/core.h>
#include <iostream>

#include <cctype>
#include <string>

namespace logic {

template <typename T>
concept Printable = requires(const T& msg) {
  { fmt::println("{}", msg)};
};

class Color {

  enum class ColorType : int {
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Purple = 35,
    Cyan = 36,
    White = 37,
    Gray = 90,
  };

public:
  ColorType type;
  std::string contents;

  constexpr Color(Printable auto contents, ColorType type)
      : type(type), contents(contents) {}

#define DEFINE_COLOR(color)                                                    \
  template <Printable T>                                                       \
  static constexpr auto color(T content) -> Color {                            \
    if constexpr (requires { std::string(content); }) {                        \
      return Color(content, ColorType::color);                                 \
    } else {                                                                   \
      return Color(std::to_string(content), ColorType::color);                 \
    }                                                                          \
  }

  DEFINE_COLOR(Black);
  DEFINE_COLOR(Red);
  DEFINE_COLOR(Blue);
  DEFINE_COLOR(Green);
  DEFINE_COLOR(Yellow);
  DEFINE_COLOR(Purple);
  DEFINE_COLOR(White);
  DEFINE_COLOR(Gray);
  DEFINE_COLOR(Cyan);


};

} // namespace logic

template <>
class fmt::formatter<logic::Color> {
public:
  constexpr auto parse (format_parse_context& ctx) { 
    return ctx.begin(); 
  }

  template <typename FormatContext>
  constexpr auto format(const logic::Color &c, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "\x1B[0;{}m{}\x1B[0m", int(c.type), c.contents);
  }
};

