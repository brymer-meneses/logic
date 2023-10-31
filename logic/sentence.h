#pragma once

#include "logic/token.h"

#include <memory>
#include <variant>

namespace logic {

struct Sentence {

  struct Atomic {
    Token value;
    explicit Atomic(Token value) : value(value) {}
  };

  struct Negated  {
    Token value;
    explicit Negated(Token value) : value(value) {}
  };

  struct Connected {
    Token connective;
    std::unique_ptr<Sentence> left;
    std::unique_ptr<Sentence> right;
    explicit Connected(Token connective, 
                       Sentence left, 
                       Sentence right) 
      : connective(connective)
      , left(std::make_unique<Sentence>(std::move(left)))
      , right(std::make_unique<Sentence>(std::move(right))) {}
  };

  using ValueType = std::variant<Atomic, Connected, Negated>;

public:
  template <typename T>
  requires std::is_constructible_v<T, ValueType>
  constexpr Sentence(T value) : value(std::move(value)) { }

private:
  ValueType value;
};

}
