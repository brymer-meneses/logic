#include "logic/scanner.h"
#include "logic/utils.h"

using namespace logic;

auto Scanner::scan() -> std::expected<std::vector<Token>, ScannerError> {
  auto tokens = std::vector<Token>{};
  while (not isAtEnd()) {
    auto token = scanNextToken();
    if (not token) {
      return std::unexpected(token.error());
    }
    tokens.push_back(*token);
  }

  tokens.push_back(buildToken(TokenType::EndOfFile));
  return tokens;
}

auto Scanner::scanNextToken() -> std::expected<Token, ScannerError> {
  auto c = advance();
  c = skipWhitespaces(c);

  switch (c) {
    case '(':
      return buildToken(TokenType::LeftParen);
    case ')':
      return buildToken(TokenType::RightParen);
    case 'P':
    case 'S':
    case 'Q':
      return buildToken(TokenType::Variable);

    default:
      if (isalpha(c)) {
        return scanKeyword();
      } else {
        return std::unexpected(ScannerError::UnexpectedCharacter(c));
      }
  }
}

static constexpr auto keywordLookup(std::string_view lexeme) -> std::expected<TokenType, ScannerError> {
  if (lexeme == "TRUE" ) {
    return TokenType::True;
  }  
  if (lexeme == "FALSE") {
    return TokenType::False;
  }

  if (lexeme == "NOT") {
    return TokenType::Not;
  }
  if (lexeme == "AND") {
    return TokenType::And;
  }
  if (lexeme == "OR") {
    return TokenType::Or;
  }
  if (lexeme == "IMPLIES") {
    return TokenType::Implies;
  }
  if (lexeme == "EQUIVALENT") {
    return TokenType::Equivalent;
  }
  return std::unexpected(ScannerError::UnexpectedKeyword(lexeme));
}

auto Scanner::scanKeyword() -> std::expected<Token, ScannerError> {
  while (std::isalpha(peek())) {
    advance();
  }
  auto lexeme = mSource.substr(mStart, mCurrent - mStart);
  auto type = TRY(keywordLookup(lexeme));
  return buildToken(type);
}

constexpr auto Scanner::buildToken(TokenType type) -> Token {
  auto linePosition = SourceLocation(mStart - mLastLine, mStart - mLastLine, mLine);
  auto lexeme = mSource.substr(mStart, mCurrent - mStart);
  mStart = mCurrent;
  return Token(type, linePosition, lexeme);
}

auto Scanner::skipWhitespaces(char c) -> char {
  while (c == ' ' or c == '\t' or c == '\r' or c == '\n') {
    mStart = mCurrent;
    if (c == '\n') {
      mLastLine = mCurrent - 1;
      mLine += 1;
    }
    c = advance();
  }
  return c;
}

auto Scanner::advance() -> char {
  if (isAtEnd()) {
    return '\0';
  }
  return mSource[mCurrent++];
}

auto Scanner::match(const char character) -> bool {
  if (peek() == character) {
    mCurrent += 1;
    return true;
  }
  return false;
}

constexpr auto Scanner::isAtEnd() const -> bool {
  return mCurrent >= mSource.length();
}

constexpr auto Scanner::peek() const -> char {
  if (isAtEnd()) {
    return '\0';
  }
  return mSource[mCurrent];
}
