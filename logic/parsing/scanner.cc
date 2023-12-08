#include "logic/parsing/scanner.h"
#include "logic/utils/macros.h"

using namespace logic;

auto Scanner::scan() -> std::expected<std::vector<Token>, ScannerError> {
  while (not isAtEnd()) {
    mStart = mCurrent;
    auto value = scanToken();
    if (not value) { return std::unexpected(value.error()); }
  }
  addToken(TokenType::EndOfFile);
  return mTokens;
}

auto Scanner::scanToken() -> std::expected<void, ScannerError> {
  auto c = advance();
  switch (c) {
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      mLastLine = mCurrent - 1;
      mLine += 1;
      break;
    case '#':
      while (peek() != '\n' and not isAtEnd()) {
        advance();
      };
      break;
    case '(':
      addToken(TokenType::LeftParen);
      break;
    case ')':
      addToken(TokenType::RightParen);
      break;
    default: 
      if (std::isalpha(c)) {
        return scanKeyword();
      } else {
        return std::unexpected(ScannerError::UnexpectedCharacter(c));
      }
  }

  return {};
}

auto Scanner::addToken(TokenType type) -> void {
  auto linePosition = SourceLocation(mStart - mLastLine, mStart - mLastLine, mLine);
  auto lexeme = mSource.substr(mStart, mCurrent - mStart);
  mTokens.emplace_back(type, linePosition, lexeme);
}

auto Scanner::scanKeyword() -> std::expected<void, ScannerError> {
  static constexpr auto keywordLookup = [](std::string_view lexeme) -> TokenType {
    if (lexeme == "TRUE" ) {
      return TokenType::True;
    } else if (lexeme == "FALSE") {
      return TokenType::False;
    } else if (lexeme == "NOT") {
      return TokenType::Not;
    } else if (lexeme == "AND") {
      return TokenType::And;
    } else if (lexeme == "OR") {
      return TokenType::Or;
    } else if (lexeme == "IMPLIES") {
      return TokenType::Implies;
    } else if (lexeme == "EQUIVALENT") {
      return TokenType::Equivalent;
    } else {
      return TokenType::Variable;
    }
  };

  while (std::isalpha(peek())) {
    advance();
  }

  auto lexeme = mSource.substr(mStart, mCurrent - mStart);
  auto type = keywordLookup(lexeme);
  if (lexeme.length() != 1 and type == TokenType::Variable) {
    return std::unexpected(ScannerError::InvalidVariableName(lexeme));
  }

  addToken(type);
  return {};
}



auto Scanner::match(char type) -> bool {
  if (peek() == type) {
    mCurrent += 1;
    return true;
  }
  return false;
}

constexpr auto Scanner::advance() -> char {
  if (isAtEnd()) {
    return '\0';
  }
  return mSource.at(mCurrent++);
}

constexpr auto Scanner::isAtEnd() const -> bool {
  return mCurrent >= mSource.length();
}

constexpr auto Scanner::peek() const -> char {
  if (isAtEnd()) {
    return '\0';
  }
  return mSource.at(mCurrent);
}
