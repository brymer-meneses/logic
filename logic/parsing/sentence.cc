#include "logic/parsing/sentence.h"
#include "logic/parsing/token.h"
#include "logic/utils/overloaded.h"

#include <string>
#include <fmt/core.h>

using namespace logic;

auto Sentence::asString(const Sentence& s) -> std::string {
  return s.accept(overloaded {
    [](const Sentence::Grouped& s) {
      return fmt::format("({})", asString(*s.sentence));
    },
    [](const Sentence::Value& s) {
      return fmt::format("{}", tokenTypeToString(s.value.type));
    },
    [](const Sentence::Negated& s) {
      return fmt::format("¬{}", asString(*s.sentence));
    },
    [](const Sentence::Variable& s) {
      return fmt::format("{}", s.identifier.lexeme);
    },
    [](const Sentence::Compound& s) {
      auto right = asString(*s.right);
      auto left = asString(*s.left);
      if (s.right->is<Sentence::Compound>()) {
        right = fmt::format("({})", right);
      }
      if (s.left->is<Sentence::Compound>()) {
        left = fmt::format("({})", left);
      }
      return fmt::format("{} {} {}", left, tokenTypeToString(s.connective.type), right);
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
