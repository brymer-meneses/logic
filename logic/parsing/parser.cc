
#include <logic/parsing/parser.h>
#include <logic/utils.h>

using namespace logic;

auto Parser::parse() -> std::expected<std::vector<Sentence>, ParserError> {
  auto sentences = std::vector<Sentence> {};
  while (not isAtEnd()) {
    auto sentence = TRY(parseSentence());
    sentences.push_back(std::move(sentence));
  }
  return sentences;
}

auto Parser::parseSentence() -> std::expected<Sentence, ParserError> {
  return parseCompoundSentence();
}

auto Parser::parseGroupedSentence() -> std::expected<Sentence, ParserError> {
  auto sentence = TRY(parseSentence());

  if (not match(TokenType::RightParen)) {
    return std::unexpected(ParserError::ExpectedToken(TokenType::RightParen, peek()));
  }

  return Sentence::Grouped(std::move(sentence));
}

auto Parser::parseCompoundSentence() -> std::expected<Sentence, ParserError> {
  auto lhs = TRY(parsePrimary());
  return parseBinaryRHS(0, std::move(lhs));
}

auto Parser::parsePrimary() -> std::expected<Sentence, ParserError> {
  if (match(TokenType::LeftParen)) {
    return parseGroupedSentence();
  }
  if (match(TokenType::Not)) {
    return parseNegatedSentence();
  }

  return parseAtomicSentence();
}

auto Parser::parseBinaryRHS(int prevPrec, Sentence lhs) -> std::expected<Sentence, ParserError> {
  constexpr static auto getTokenPrecedence = [](TokenType type) -> int {
    switch (type) {
      case TokenType::Equivalent:
        return 10;
      case TokenType::Implies:
        return 20;
      case TokenType::Or:
        return 30;
      case TokenType::And:
        return 40;
      default:
        return -1;
    }
  };

  while (true) {
    auto tokenPrec = getTokenPrecedence(peek().type);
    if (tokenPrec < prevPrec) {
      return lhs;
    }

    auto binOp = advance();
    auto rhs = TRY(parsePrimary());

    auto nextPrec = getTokenPrecedence(peek().type);
    if (tokenPrec <= nextPrec) {
      // NOTE: We prefer right associativity so we make the 'tokenPrec' which is the
      //       current operator precedence, and prefer the other precedence.
      //       This is needed since '=>' and '<=>' are right associative.
      rhs = TRY(parseBinaryRHS(tokenPrec-1, std::move(rhs)));
    }

    lhs = Sentence::Compound(binOp, std::move(lhs), std::move(rhs));
  }
}

auto Parser::parseNegatedSentence() -> std::expected<Sentence, ParserError> {
  auto sentence = TRY(parsePrimary());
  return Sentence::Negated(std::move(sentence));
}

auto Parser::parseAtomicSentence() -> std::expected<Sentence, ParserError> {

  if (match({TokenType::True, TokenType::False})) {
    return Sentence::Value(peekPrevious());
  }

  if (match(TokenType::Variable)) {
    return Sentence::Variable(peekPrevious());
  }

  RAISE_INTERNAL_ERROR("Unreachable");
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

constexpr auto Parser::advance() -> const Token& {
  if (isAtEnd()) {
    return mTokens.back();
  }

  return mTokens[mCurrent++];
}
