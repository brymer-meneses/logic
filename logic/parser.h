#pragma once

#include <cstddef>
#include <vector>
#include <expected>
#include <variant>

#include <logic/token.h>
#include <logic/sentence.h>

namespace logic {

class ParserError {

public:
  struct ExpectedToken {
    TokenType expected;
    Token got;

    explicit ExpectedToken(TokenType expected, Token got) 
      : expected(expected)
      , got(got) {}
  };

private:
  using ValueType = std::variant<ExpectedToken>;
  ValueType value;

public:
  template <typename T>
  requires std::is_constructible_v<ValueType, T>
  constexpr ParserError(T value) : value(value) {}

  constexpr auto accept(auto visitor) -> decltype(auto) {
    return std::visit(visitor, value);
  } 
};

class Parser {

  size_t mCurrent = 0;
  std::vector<Token> mTokens;

public:
  constexpr Parser(std::vector<Token> tokens) 
    : mTokens(std::move(tokens)) {}

  auto parse() -> std::expected<std::vector<Sentence>, ParserError>;

private:
  auto parseSentence() -> std::expected<Sentence, ParserError>;

  auto parseComplexSentence() -> std::expected<Sentence, ParserError>;
  auto parseConnectedSentence() -> std::expected<Sentence, ParserError>;
  auto parseGroupingSentence() -> std::expected<Sentence, ParserError>;

  auto parseAtomicSentence() -> std::expected<Sentence, ParserError>;

  constexpr auto match(TokenType) -> bool;
  constexpr auto match(std::initializer_list<TokenType> tokens) -> bool;

  constexpr auto peek() const -> const Token&;
  constexpr auto peekPrevious() const -> const Token&;

  constexpr auto check() const -> bool;
  constexpr auto isAtEnd() const -> bool;


};

}
