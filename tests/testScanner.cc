#include <print>
#include <ranges>

#include <gtest/gtest.h>

#include <logic/parsing/scanner.h>
#include <logic/parsing/token.h>

#include <logic/utils/macros.h>
#include <logic/utils/overloaded.h>

using namespace logic;



TEST(Scanner, TestKeywords) {

  auto scanner = Scanner("TRUE FALSE NOT AND OR IMPLIES EQUIVALENT P Q S ( )");
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    FAIL() << tokens.error().accept(overloaded {
        [](const ScannerError::UnexpectedKeyword &e) {
          return std::format("Unexpected keyword `{}`", e.keyword);
        },
        [](const ScannerError::InvalidVariableName &e) {
          return std::format("Invalid Variable name`{}`", e.name);
        },
        [](const ScannerError::UnexpectedCharacter &e) {
          return std::format("Unexpected character `{}`", e.character);
        }}
      );
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
    EXPECT_EQ(token.type, expectedType);
  }

}



