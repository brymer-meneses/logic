#include "logic/parsing/sentence.h"
#include "logic/parsing/token.h"

#include <string>

auto logic::sentenceAsString(const Sentence& s) -> std::string {
  return s.accept(overloaded {
    [](const Sentence::Grouped& s) {
      return std::format("({})",sentenceAsString(*s.sentence));
    },
    [](const Sentence::Value& s) {
      return std::format("{}", tokenTypeToString(s.value.type));
    },
    [](const Sentence::Negated& s) {
      return std::format("¬{}",sentenceAsString(*s.sentence));
    },
    [](const Sentence::Variable& s) {
      return std::format("{}", s.identifier.lexeme);
    },
    [](const Sentence::Compound& s) {
      return std::format("({} {} {})", 
                         sentenceAsString(*s.left), 
                         tokenTypeToString(s.connective.type), 
                         sentenceAsString(*s.right));
    },
  });

}
