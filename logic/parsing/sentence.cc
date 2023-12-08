#include "logic/parsing/sentence.h"
#include "logic/parsing/token.h"
#include "logic/utils/overloaded.h"

#include <string>
#include <format>

auto logic::sentenceAsString(const Sentence& s) -> std::string {
  return s.accept(overloaded {
    [](const Sentence::Grouped& s) {
      return std::format("({})", sentenceAsString(*s.sentence));
    },
    [](const Sentence::Value& s) {
      return std::format("{}", tokenTypeToString(s.value.type));
    },
    [](const Sentence::Negated& s) {
      return std::format("¬{}", sentenceAsString(*s.sentence));
    },
    [](const Sentence::Variable& s) {
      return std::format("{}", s.identifier.lexeme);
    },
    [](const Sentence::Compound& s) {
      auto right = sentenceAsString(*s.right);
      auto left = sentenceAsString(*s.left);
      if (s.right->is<Sentence::Compound>()) {
        right = std::format("({})", right);
      }
      if (s.left->is<Sentence::Compound>()) {
        left = std::format("({})", left);
      }
      return std::format("{} {} {}", left, tokenTypeToString(s.connective.type), right);
    },
  });

}
