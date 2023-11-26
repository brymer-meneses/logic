#pragma once
#include <cstddef>
#include <string_view>
#include <vector>
#include <expected>
#include <variant>
#include <format>

#include "logic/parsing/token.h"

namespace logic {

struct ScannerError {
public:
  struct UnexpectedCharacter {
    char character;
    constexpr UnexpectedCharacter(char character) : character(character) {}
  };

  struct UnexpectedKeyword {
    std::string_view keyword;
    constexpr UnexpectedKeyword(std::string_view keyword) : keyword(keyword) {}
  };

private:
  using ValueType = std::variant<UnexpectedCharacter, UnexpectedKeyword>;
  ValueType value;

public:
  template <typename T>
  requires std::is_constructible_v<ValueType, T>
  constexpr ScannerError(T value) : value(value) {}

  constexpr auto accept(auto visitor) const -> decltype(auto) {
    return std::visit(visitor, value);
  } 
};

class Scanner {
  size_t mStart = 0;
  size_t mCurrent = 0;
  size_t mLine = 1;

  size_t mLastLine = 0;

  std::string_view mSource;

public:
  constexpr Scanner(std::string_view source) 
    : mSource(source) {}

  auto scan() -> std::expected<std::vector<Token>, ScannerError>;

private:
  auto scanNextToken() -> std::expected<Token, ScannerError>;
  auto scanKeyword() -> std::expected<Token, ScannerError>;

  auto skipWhitespaces(char c) -> char;

  auto advance() -> char;
  auto match(const char) -> bool;

  constexpr auto isAtEnd() const -> bool;
  constexpr auto peek() const -> char;
  constexpr auto buildToken(TokenType) -> Token;


};

}
