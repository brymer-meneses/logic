
#include <logic/parser.h>

using namespace logic;

auto Parser::parse() -> std::expected<std::vector<Sentence>, ParserError> {
  auto sentences = std::vector<Sentence> {};
  while (not isAtEnd()) {
    auto sentence = parseSentence();
    if (not sentence) {
      return std::unexpected(sentence.error());
    }
    sentences.push_back(std::move(*sentence));
  }
  return sentences;
}

auto Parser::parseSentence() -> std::expected<Sentence, ParserError> {
  
}

auto Parser::parseConnectedSentence() -> std::expected<Sentence, ParserError> {

}

auto Parser::parseComplexSentence() -> std::expected<Sentence, ParserError> {

}

auto Parser::parseAtomicSentence() -> std::expected<Sentence, ParserError> {
  if (match(TokenType::Variable)) {

  }
}

constexpr auto Parser::isAtEnd() const -> bool {
  return mCurrent >= mTokens.size();
}

constexpr auto Parser::peek() const -> const Token& {
  if (isAtEnd()) {
    return mTokens.back();
  }
  return mTokens[mCurrent];
}

constexpr auto Parser::match(TokenType type) -> bool {
  if (peek().type == type) {
    mCurrent += 1;
    return true;
  }

  return false;
}

constexpr auto Parser::match(std::initializer_list<TokenType> types) -> bool {

  for (const auto& type : types) {
    if (peek().type == type) {
      mCurrent += 1;
      return true;
    }
  }

  return false;
}
