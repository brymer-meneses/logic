#pragma once

#include <string_view>
#include <cstddef>

#include "sourceLocation.h"

namespace logic {

enum class TokenType {
  LeftParen,
  RightParen,
  Boolean,
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
};

}
