#pragma once

#include <cstddef>
#include <vector>
#include <expected>
#include <variant>

#include <logic/parsing/token.h>
#include <logic/parsing/sentence.h>

namespace logic {

class ParserError {

public:
  struct UnexpectedToken {
    TokenType expected;
    Token got;
    SourceLocation location;

    explicit UnexpectedToken(TokenType expected, Token got, SourceLocation location) 
      : expected(expected)
      , got(got) 
      , location(location) {}
  };

  struct ExpectedSentence {
    SourceLocation location;
    explicit ExpectedSentence(SourceLocation location) 
      : location(location) {}
  };

private:
  using ValueType = std::variant<UnexpectedToken, ExpectedSentence>;
  ValueType value;

public:
  template <typename T>
  requires std::is_constructible_v<ValueType, T>
  constexpr ParserError(T value) : value(value) {}

  constexpr auto accept(auto visitor) const -> decltype(auto) {
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
  auto parseAssignmentSentence() -> std::expected<Sentence, ParserError>;

  auto parseGroupedSentence() -> std::expected<Sentence, ParserError>;

  auto parseCompoundSentence() -> std::expected<Sentence, ParserError>;
  auto parseNegatedSentence() -> std::expected<Sentence, ParserError>;
  auto parseAtomicSentence() -> std::expected<Sentence, ParserError>;

  auto parseBinaryRHS(int, Sentence) -> std::expected<Sentence, ParserError>;
  auto parsePrimary() -> std::expected<Sentence, ParserError>;

  constexpr auto getCurrentLocation() const -> SourceLocation;
  constexpr auto advance() -> const Token&;

  constexpr auto match(TokenType) -> bool;
  constexpr auto match(std::initializer_list<TokenType> tokens) -> bool;

  constexpr auto peek() const -> const Token&;
  constexpr auto peekPrevious() const -> const Token&;

  constexpr auto isAtEnd() const -> bool;


};

}
