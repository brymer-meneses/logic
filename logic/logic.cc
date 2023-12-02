
#include "logic/logic.h"
#include "logic/parsing/parser.h"
#include "logic/utils/overloaded.h"


using namespace logic;

auto Logic::run(std::string_view source) -> void {
  auto scanner = Scanner(source);
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    Logic::report(tokens.error());
    exit(1);
  }

  auto parser = Parser(std::move(*tokens));
  auto sentences = parser.parse();

  if (not sentences.has_value()) {
    Logic::report(sentences.error());
  }

  for (const auto& sentence : *sentences) {
    auto evaluator = Evaluator();
    auto value = evaluator.evaluate(sentence);
    if (not value.has_value()) {
      Logic::report(value.error());
    }
  }
}

auto Logic::runFile(std::string_view filename) -> void {

}

auto Logic::report(const ParserError& e) -> void {
  auto message = e.accept(overloaded {
    [](const ParserError::ExpectedToken& e) {
      return std::format("Unexpected token {}, expected {}", e.got.lexeme, tokenTypeToString(e.expected));
    }
    });
  std::println(stderr, "{}", message);
}

auto Logic::report(const ScannerError& e) -> void {
  auto message = e.accept(overloaded {
    [](const ScannerError::UnexpectedKeyword &e) {
      return std::format("Unexpected keyword `{}`", e.keyword);
    },
    [](const ScannerError::UnexpectedCharacter &e) {
      return std::format("Unexpected character `{}`", e.character);
    }
  });
  std::println(stderr, "{}", message);
}

auto Logic::report(const EvaluatorError& e) -> void {

}
