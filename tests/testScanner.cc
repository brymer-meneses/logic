#include <ranges>

#include <gtest/gtest.h>

#include <logic/parsing/scanner.h>
#include <logic/parsing/token.h>

#include <logic/utils/macros.h>
#include <logic/utils/overloaded.h>

#include <tests/printer.h>

#include "reporter.h"

using namespace logic;

TEST(Scanner, TestKeywords) {

  auto scanner = Scanner("TRUE FALSE NOT AND OR IMPLIES EQUIVALENT P Q S ( )");
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    FAIL() << report(tokens.error());
  }

  auto expectedTokens = std::initializer_list<TokenType> {
    TokenType::True,
    TokenType::False,
    TokenType::Not,
    TokenType::And,
    TokenType::Or,
    TokenType::Implies,
    TokenType::Equivalent,
    TokenType::Variable,
    TokenType::Variable,
    TokenType::Variable,
    TokenType::LeftParen,
    TokenType::RightParen,
  };

  for (const auto& [token, expectedType] : std::views::zip(*tokens, expectedTokens)) {
    EXPECT_EQ(token.type, expectedType) << token.lexeme;
  }

}


TEST(Scanner, TestSkipComments) {

  auto scanner = Scanner("# \nTRUE FALSE NOT AND OR IMPLIES EQUIVALENT P Q S ( )");
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    FAIL() << report(tokens.error());
  }

  auto expectedTokens = std::initializer_list<TokenType> {
    TokenType::True,
    TokenType::False,
    TokenType::Not,
    TokenType::And,
    TokenType::Or,
    TokenType::Implies,
    TokenType::Equivalent,
    TokenType::Variable,
    TokenType::Variable,
    TokenType::Variable,
    TokenType::LeftParen,
    TokenType::RightParen,
  };

  for (const auto& [token, expectedType] : std::views::zip(*tokens, expectedTokens)) {
    EXPECT_EQ(token.type, expectedType) << token.lexeme;
  }
}
