
#include "logic/evaluator.h"
#include "logic/utils.h"

#include <vector>

using namespace logic;

auto Evaluator::evaluate(const Sentence& sentence) -> Value {
  return sentence.accept(
    overloaded {
      [](const Sentence::Variable& s) {
        // TOOO
        return Value(false);
      },
      [](const Sentence::Value& s) {
        // TOOO
        return Value(false);
      },
      [this](const Sentence::Negated& s) {
        return not evaluate(*s.sentence);
      },
      [this](const Sentence::Compound& s){
        switch (s.connective.type) {
          case TokenType::And:
            return evaluate(*s.left) and evaluate(*s.right);
          case TokenType::Or:
            return evaluate(*s.left) or evaluate(*s.right);
          return Value(false);
        }
      },
      [](const auto& s1) {
        return Value(false);
      }
    }
  );
}

auto Evaluator::evaluate(std::vector<Sentence> sentences) -> Value {

}
