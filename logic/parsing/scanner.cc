#include "logic/parsing/scanner.h"
#include "logic/utils/macros.h"
#include "logic/utils/utils.h"

#include <ranges>

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
      mLineStart = mCurrent;
      mLine += 1;
      break;
    case '#':
      while (peek() != '\n' and not isAtEnd()) {
        advance();
      };
      break;
    case '=':
      addToken(TokenType::Equal);
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
        return std::unexpected(ScannerError::UnexpectedCharacter(c, getCurrentLocation()));
      }
  }

  return {};
}

auto Scanner::addToken(TokenType type) -> void {
  auto linePosition = getCurrentLocation();
  auto lexeme = mSource.substr(mStart, mCurrent - mStart);
  mTokens.emplace_back(type, linePosition, lexeme);
}

auto Scanner::scanKeyword() -> std::expected<void, ScannerError> {

  static constexpr auto keywordLookup = [](std::string_view lexeme, SourceLocation location) 
    -> std::expected<TokenType, ScannerError> {

    if (lexeme.length() == 1) {
      return TokenType::Variable;
    }

    auto keywords = {"TRUE", "FALSE",   "NOT",       "AND",
                     "OR",   "IMPLIES", "EQUIVALENT"};
    auto tokens = {TokenType::True,      TokenType::False, TokenType::Not,
                   TokenType::And,       TokenType::Or,    TokenType::Implies,
                   TokenType::Equivalent};

    for (const auto& [keyword, tokenType] : std::views::zip(keywords, tokens)) {
      if (keyword == lexeme) {
        return tokenType;
      }
    }

    for (const auto& keyword : keywords) {
      if (stringToUpper(lexeme) == keyword) {
          return std::unexpected(ScannerError::InvalidKeywordFormat(lexeme, location));
      }
    }

    auto suggestion = std::min_element(
        keywords.begin(), keywords.end(),
        [&](const std::string_view &a, const std::string_view &b) {
          return levenshteinDistance(lexeme, a) <
                 levenshteinDistance(lexeme, b);
        });

    return std::unexpected(ScannerError::UnexpectedKeyword(lexeme, *suggestion, location));
  };

  while (std::isalpha(peek())) {
    advance();
  }

  auto lexeme = mSource.substr(mStart, mCurrent - mStart);
  auto type = TRY(keywordLookup(lexeme, getCurrentLocation()));
  if (lexeme.length() != 1 and type == TokenType::Variable) {
    return std::unexpected(ScannerError::InvalidVariableName(lexeme, getCurrentLocation()));
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

constexpr auto Scanner::getCurrentLocation() const -> SourceLocation {
  return SourceLocation(mStart - mLineStart, mCurrent - mLineStart, mLine, mFilename);
}
