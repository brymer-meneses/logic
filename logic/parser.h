#pragma once

#include <cstddef>
#include <vector>
#include <expected>

#include <logic/token.h>
#include <logic/sentence.h>

namespace logic {

class ParserError {

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

  auto parseAtomicSentence() -> std::expected<Sentence, ParserError>;


  constexpr auto match(TokenType) -> bool;
  constexpr auto match(std::initializer_list<TokenType> tokens) -> bool;

  constexpr auto peek() const -> const Token&;
  constexpr auto check() const -> bool;
  constexpr auto isAtEnd() const -> bool;


};

}
