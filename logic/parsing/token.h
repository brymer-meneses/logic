#pragma once

#include <string_view>
#include <cstddef>

#include "sourceLocation.h"

namespace logic {

enum class TokenType {
  LeftParen,
  RightParen,
  True,
  False,
  Variable,
  Not,
  And,
  Or,
  Implies,
  Equivalent,
  EndOfFile,
};

struct Token {

  TokenType type;
  SourceLocation location;
  std::string_view lexeme;

  constexpr Token(TokenType type, SourceLocation location, std::string_view lexeme) 
    : type(type), location(location), lexeme(lexeme) {}

  friend constexpr auto operator==(const Token& t1, const Token& t2) -> bool {
    return t1.type == t2.type and t1.lexeme == t2.lexeme;
  };
};

constexpr auto tokenTypeToString(TokenType type) -> std::string_view {
  switch (type) {
    case TokenType::LeftParen:
      return "(";
    case TokenType::RightParen:
      return ")";
    case TokenType::True:
      return "TRUE";
    case TokenType::False:
      return "FALSE";
    case TokenType::Variable:
      return "VARIABLE";
    case TokenType::Not:
      return "¬";
    case TokenType::And:
      return "∧";
    case TokenType::Or:
      return "∨";
    case TokenType::Implies:
      return "=>";
    case TokenType::Equivalent:
      return "<=>";
    case TokenType::EndOfFile:
      return "EOF";
  }

}

}
