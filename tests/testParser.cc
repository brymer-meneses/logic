#include <gtest/gtest.h>

#include <logic/sentence.h>
#include <logic/scanner.h>
#include <logic/token.h>
#include <logic/parser.h>
#include <logic/utils.h>

using namespace logic;

auto verifySentence(std::string_view source, Sentence sentence) -> void {
  auto scanner = Scanner(source);
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    FAIL() << tokens.error().accept(overloaded {
        [](const ScannerError::UnexpectedKeyword &e) {
          return std::format("Unexpected keyword `{}`", e.keyword);
        },
        [](const ScannerError::UnexpectedCharacter &e) {
          return std::format("Unexpected character `{}`", e.character);
        }}
      );
  }

  auto parser = Parser(std::move(*tokens));
  auto sentences = parser.parse();

  if (not sentences.has_value()) {
    FAIL() << sentences.error().accept(overloaded {
      [](const ParserError::ExpectedToken& e) {
        return std::format("Unexpected token {}, expected {}", e.got.lexeme, tokenTypeToString(e.expected));
      }
    });
  }

  EXPECT_EQ(sentence, sentences->at(0));
} 

static constexpr auto DUMMY_LOCATION = SourceLocation(0, 0, 0);

TEST(Parser, TestVariableSentence) {
  auto sentence = Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P"));
  verifySentence("P", sentence);
}

TEST(Parser, TestConnectedSentence) {
  auto sentence = Sentence::Connected(
    Token(TokenType::Implies, DUMMY_LOCATION, "IMPLIES"), 
    Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P")),
    Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "Q")));

  verifySentence("P IMPLIES Q", std::move(sentence));
}

TEST(Parser, TestNegatedSentence) {
  auto sentence = Sentence::Negated(
    Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P"))
  );

  verifySentence("NOT P", std::move(sentence));
}

TEST(Parser, TestComplexSentence) {

  auto sentence = Sentence::Grouped(
      Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P"))
  );

  verifySentence("( P )", std::move(sentence));
}
