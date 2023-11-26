#include <iostream>

#include "logic/parsing/sentence.h"
#include "logic/evaluation/value.h"

#include "printer.h"

auto logic::operator<<(std::ostream& stream, const Sentence& sentence) -> std::ostream& {
  return stream << sentenceAsString(sentence);
}

auto logic::operator<<(std::ostream& stream, const Value& value) -> std::ostream& {
  return stream << std::format("Value({})", value.data);
}

