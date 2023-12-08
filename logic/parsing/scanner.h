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
    SourceLocation location;
    constexpr UnexpectedCharacter(char character, SourceLocation location) : character(character), location(location) {}
  };

  struct UnexpectedKeyword {
    std::string_view keyword;
    SourceLocation location;
    constexpr UnexpectedKeyword(std::string_view keyword, SourceLocation location) : keyword(keyword), location(location) {}
  };
  struct InvalidVariableName {
    std::string_view name;
    SourceLocation location;
    constexpr InvalidVariableName(std::string_view name, SourceLocation location) : name(name), location(location) {}
  };

private:
  using ValueType = std::variant<UnexpectedCharacter, UnexpectedKeyword, InvalidVariableName>;
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
  std::vector<Token> mTokens;

  std::string_view mSource;
  std::string_view mFilename;

public:
  constexpr Scanner(std::string_view source, std::string_view filename="REPL") 
    : mSource(source), mFilename(filename) {}

  auto scan() -> std::expected<std::vector<Token>, ScannerError>;

private:
  auto scanToken() -> std::expected<void, ScannerError>;
  auto scanKeyword() -> std::expected<void, ScannerError>;

  auto addToken(TokenType) -> void;
  auto match(char) -> bool;

  constexpr auto advance() -> char;
  constexpr auto peek() const -> char;
  constexpr auto isAtEnd() const -> bool;

  constexpr auto getCurrentLocation() const -> SourceLocation;

};

}
