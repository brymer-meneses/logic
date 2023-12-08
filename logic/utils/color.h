#pragma once

#include <cctype>
#include <format>
#include <string>
#include <print>

namespace logic {

template <typename T>
concept Printable = requires(const T& msg) {
  { std::println("{}", msg)};
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
  template <Printable T> static constexpr auto color(T content)->Color {       \
    if constexpr (std::is_same_v<std::string, T> or                            \
                  std::is_same_v<std::string_view, T> or                       \
                  std::is_same_v<const char *, T>) {                           \
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

template <> struct std::formatter<logic::Color> {

  template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(const logic::Color &c, FormatContext &ctx) const {
    return std::format_to(ctx.out(), "\e[0;{}m{}\e[0m", int(c.type),
                          c.contents);
  }
};
