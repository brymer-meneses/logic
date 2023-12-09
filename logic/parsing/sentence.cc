#include "logic/parsing/sentence.h"
#include "logic/parsing/token.h"
#include "logic/utils/overloaded.h"

#include <string>
#include <format>

using namespace logic;

auto Sentence::asString(const Sentence& s) -> std::string {
  return s.accept(overloaded {
    [](const Sentence::Grouped& s) {
      return std::format("({})", asString(*s.sentence));
    },
    [](const Sentence::Value& s) {
      return std::format("{}", tokenTypeToString(s.value.type));
    },
    [](const Sentence::Negated& s) {
      return std::format("¬{}", asString(*s.sentence));
    },
    [](const Sentence::Variable& s) {
      return std::format("{}", s.identifier.lexeme);
    },
    [](const Sentence::Compound& s) {
      auto right = asString(*s.right);
      auto left = asString(*s.left);
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

auto Sentence::location() const -> SourceLocation {
  return accept(overloaded {
    [](const Sentence::Grouped& s) {
      return s.sentence->location();
    },
    [](const Sentence::Value& s) {
      return s.value.location;
    },
    [](const Sentence::Negated& s) {
      return s.sentence->location();
    },
    [](const Sentence::Variable& s) {
      return s.identifier.location;
    },
    [](const Sentence::Compound& s) {
      return s.right->location() + s.connective.location + s.left->location();
    },
  });
}
