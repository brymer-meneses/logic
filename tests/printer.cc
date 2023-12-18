#include <fmt/core.h>
#include <fmt/ranges.h>

#include "logic/parsing/sentence.h"
#include "logic/evaluation/value.h"

#include "printer.h"

auto logic::operator<<(std::ostream& stream, const Sentence& sentence) -> std::ostream& {
  return stream << Sentence::asString(sentence);
}

auto logic::operator<<(std::ostream& stream, const Value& value) -> std::ostream& {
  return stream << fmt::format("Value({})", fmt::join(value.data, ", "));
}

auto logic::operator<<(std::ostream& stream, const TokenType& value) -> std::ostream& {
  return stream << fmt::format("{}", tokenTypeToString(value));
}

