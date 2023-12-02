#include <gtest/gtest.h>

#include <logic/parsing/sentence.h>
#include <logic/parsing/scanner.h>
#include <logic/parsing/token.h>
#include <logic/parsing/parser.h>

#include <logic/utils/macros.h>
#include <logic/utils/overloaded.h>

#include "tests/printer.h"

using namespace logic;

auto verifySentence(std::string_view source, Sentence sentence) -> void {
  auto scanner = Scanner(source);
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

TEST(Parser, TestNegatedSentence) {
  auto sentence = Sentence::Negated(
    Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P"))
  );

  verifySentence("NOT P", std::move(sentence));
}

TEST(Parser, TestCompoundSentence) {
  auto sentence = Sentence::Compound(
    Token(TokenType::Implies, DUMMY_LOCATION, "IMPLIES"), 
    Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P")),
    Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "Q")));

  verifySentence("P IMPLIES Q", std::move(sentence));
}

TEST(Parser, TestComplexSentence) {
  auto s1 = Sentence::Negated(
      Sentence::Grouped(
        Sentence::Compound(
          Token(TokenType::Implies, DUMMY_LOCATION, "IMPLIES"), 
          Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P")),
          Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "Q"))
        )
      )
  );
  
  verifySentence("NOT ( P IMPLIES Q )", std::move(s1));

  auto s2 = Sentence::Compound(
    Token(TokenType::Implies, DUMMY_LOCATION, "IMPLIES"), 
    Sentence::Negated(Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P"))),
    Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "Q"))
  );

  verifySentence("NOT P IMPLIES Q", std::move(s2));
}

TEST(Parser, TestOperatorPrecedence) {
  auto sentence = Sentence::Compound(
    Token(TokenType::Or, DUMMY_LOCATION, "OR"), 
    Sentence::Compound(
      Token(TokenType::And, DUMMY_LOCATION, "AND"), 
      Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "P")),
      Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "Q"))
    ),
    Sentence::Variable(
      Token(TokenType::Variable, DUMMY_LOCATION, "P")
    )
  );
  verifySentence("P AND Q OR P", std::move(sentence));
}

TEST(Parser, TestImplicationAssociativity) {
  auto sentence = Sentence::Compound(
    Token(TokenType::Implies, DUMMY_LOCATION, "IMPLIES"),
    Sentence::Variable(
      Token(TokenType::Variable, DUMMY_LOCATION, "P")
    ),
    Sentence::Compound(
      Token(TokenType::Implies, DUMMY_LOCATION, "IMPLIES"), 
      Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "Q")),
      Sentence::Variable(Token(TokenType::Variable, DUMMY_LOCATION, "S"))
    )
  );
  verifySentence("P IMPLIES Q IMPLIES S", std::move(sentence));
}
