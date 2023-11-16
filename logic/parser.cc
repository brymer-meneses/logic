
#include <logic/parser.h>
#include <logic/utils.h>

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
  if (match(TokenType::LeftParen)) {
    return parseGroupingSentence();
  }

  return parseComplexSentence();
}

auto Parser::parseConnectedSentence() -> std::expected<Sentence, ParserError> {
  auto sentence = parseAtomicSentence();

  if (not sentence) {
    return std::unexpected(sentence.error());
  }

  while (match({TokenType::Implies, TokenType::And, TokenType::Or, TokenType::Equivalent})) {
    auto op = peekPrevious();
    auto right = parseAtomicSentence();
    sentence = Sentence::Connected(op, std::move(*sentence), std::move(*right));
  }

  return sentence;
}

auto Parser::parseGroupingSentence() -> std::expected<Sentence, ParserError> {
  auto sentence = parseSentence();

  if (not match(TokenType::RightParen)) {
    return std::unexpected(ParserError::ExpectedToken(TokenType::RightParen, peek()));
  }

  return sentence;
}

auto Parser::parseComplexSentence() -> std::expected<Sentence, ParserError> {
  return parseConnectedSentence();
}


auto Parser::parseAtomicSentence() -> std::expected<Sentence, ParserError> {
  if (match(TokenType::Variable)) {
    return Sentence::Variable(peekPrevious());
  }

  if (match(TokenType::True) or match(TokenType::False)) {
    return Sentence::Value(peekPrevious());
  }

  __builtin_unreachable();
}

constexpr auto Parser::isAtEnd() const -> bool {
  return mCurrent >= mTokens.size() or mTokens.at(mCurrent).type == TokenType::EndOfFile;
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

constexpr auto Parser::peekPrevious() const -> const Token& {
  if (mCurrent == 0) {
    RAISE_INTERNAL_ERROR("Tried to call peekPrevious when mCurrent is 0");
  }
  return mTokens[mCurrent-1];
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
