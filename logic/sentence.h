#pragma once

#include "logic/token.h"

#include <memory>
#include <variant>

namespace logic {

struct Sentence {

  struct Variable {
    Token identifier;
    explicit Variable(Token value) : identifier(value) {}

    friend constexpr auto operator==(const Variable& v1, const Variable& v2) -> bool = default;
  };

  struct Value {
    Token value;
    explicit Value(Token value) : value(value) {}

    friend constexpr auto operator==(const Value&, const Value&) -> bool = default;
  };

  struct Negated  {
    std::unique_ptr<Sentence> sentence;
    explicit Negated(Sentence value)
        : sentence(std::make_unique<Sentence>(std::move(value))) {}

    friend constexpr auto operator==(const Negated& n1, const Negated& n2) -> bool {
      return *n1.sentence == *n2.sentence;
    }
  };

  struct Grouped {
    std::unique_ptr<Sentence> sentence;
    explicit Grouped(Sentence value)
        : sentence(std::make_unique<Sentence>(std::move(value))) {}

    friend constexpr auto operator==(const Grouped& n1, const Grouped& n2) -> bool {
      return *n1.sentence == *n2.sentence;
    }

  };

  struct Compound {
    Token connective;
    std::unique_ptr<Sentence> left;
    std::unique_ptr<Sentence> right;
    explicit Compound(Token connective, Sentence left, Sentence right)
        : connective(connective)
        , left(std::make_unique<Sentence>(std::move(left)))
        , right(std::make_unique<Sentence>(std::move(right))) {}

    friend constexpr auto operator==(const Compound& s1, const Compound& s2) -> bool {
      return s1.connective == s2.connective and *s1.left == *s2.left and *s1.right == *s2.right;
    };
  };

  using ValueType = std::variant<Variable, Compound, Negated, Value, Grouped>;

public:
  Sentence(Compound value) : value(std::move(value)) { }
  Sentence(Grouped value) : value(std::move(value)) { }
  Sentence(Negated value) : value(std::move(value)) { }
  Sentence(Value value) : value(std::move(value)) { }
  Sentence(Variable value) : value(std::move(value)) { }

  friend constexpr auto operator==(const Sentence&, const Sentence&) -> bool = default;

private:
  ValueType value;

};

}
