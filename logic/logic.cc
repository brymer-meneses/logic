
#include "logic/logic.h"
#include "logic/parsing/parser.h"
#include "logic/utils/overloaded.h"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace logic;

auto Logic::run(std::string_view source) -> void {
  auto scanner = Scanner(source);
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    Logic::report(tokens.error());
    return;
  }

  auto parser = Parser(std::move(*tokens));
  auto sentences = parser.parse();

  if (not sentences.has_value()) {
    Logic::report(sentences.error());
    return;
  }

  for (const auto& sentence : *sentences) {
    auto evaluator = Evaluator();
    auto value = evaluator.evaluate(sentence);
    if (not value.has_value()) {
      Logic::report(value.error());
      return;
    }

    evaluator.printEvaluation();
  }
}

auto Logic::runREPL() -> void {
  while (true) {
    std::string source;
    std::print(">>> ");
    
    // Break the loop on EOF or other error
    if (not std::getline(std::cin, source)) { break; }

    run(source);
  }
}

auto Logic::runFile(std::string_view filename) -> void {
  std::ifstream file(filename);
  std::stringstream source;

  if (file.fail()) {
    std::println(stderr, "LOGIC: File `{}` does not exist", filename);
    exit(1);
  }

  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      source << line << "\n";
    }
  }
  run(source.str());
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
    [](const ScannerError::InvalidVariableName &e) {
      return std::format("Invalid variable name `{}`. Variables must have length 1", e.name);
    },
    [](const ScannerError::UnexpectedCharacter &e) {
      return std::format("Unexpected character `{}`", int(e.character));
    }
  });
  std::println(stderr, "{}", message);
}

auto Logic::report(const EvaluatorError& e) -> void {

}
