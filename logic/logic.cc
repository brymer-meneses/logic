
#include "logic/logic.h"
#include "logic/parsing/parser.h"
#include "logic/utils/overloaded.h"
#include "logic/utils/color.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <utility>

using namespace logic;


auto Logic::run(std::string_view source) -> void {
  auto scanner = Scanner(source, "REPL");
  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    Logic::report(tokens.error(), source);
    return;
  }

  auto parser = Parser(std::move(*tokens));
  auto sentences = parser.parse();

  if (not sentences.has_value()) {
    Logic::report(sentences.error(), source);
    return;
  }

  for (const auto& sentence : *sentences) {
    auto evaluator = Evaluator();
    auto value = evaluator.evaluate(sentence);
    if (not value.has_value()) {
      Logic::report(value.error(), source);
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

auto Logic::report(const ParserError& e, std::string_view source) -> void {
  auto location = e.accept([](auto& error){return error.location;}) ;
  auto message = e.accept(overloaded {
    [](const ParserError::ExpectedToken& e) {
      return std::format("Unexpected token {}, expected {}", e.got.lexeme, tokenTypeToString(e.expected));
    }
  });
  reportInternal(message, location, source);
}

auto Logic::report(const ScannerError& e, std::string_view source) -> void {
  auto location = e.accept([](auto& error){return error.location;}) ;
  auto message = e.accept(overloaded {
    [](const ScannerError::UnexpectedKeyword &e) {
      return std::format("Unexpected keyword `{}`", e.keyword);
    },
    [](const ScannerError::InvalidVariableName &e) {
      return std::format("Invalid variable name `{}`. Variables must have length 1", e.name);
    },
    [](const ScannerError::UnexpectedCharacter &e) {
      return std::format("Unexpected character `{}`", e.character);
    }
  });

  reportInternal(message, location, source);
}

auto Logic::report(const EvaluatorError& e, std::string_view) -> void {

}


auto Logic::reportInternal(std::string_view message, SourceLocation location, std::string_view source) -> void {
  static constexpr auto getNthLine = [](std::string_view inputString, int nthLine) -> std::string {
    std::string input(inputString);
    std::istringstream iss(input);
    std::string line;
    int currentLine = 0;
    while (currentLine < nthLine and std::getline(iss, line)) {
      ++currentLine;
    }
    return (currentLine == nthLine) ? line : "";
  };
  static constexpr auto leftPad = [](std::string_view string, size_t length) -> std::string {
    return std::format("{}{}", std::string(length, ' '), string);
  };
  static constexpr auto highlightArrows = [](size_t length) -> std::string {
    auto formatStr = std::format("{{:^^{}}}", length);
    return std::vformat(formatStr, std::make_format_args(""));
  };

  if (location.filename != "REPL")  {
    std::println("{}:{}:{}", location.filename, location.line, location.start);
  }

  auto locationLength = std::to_string(location.line).length();
  auto padding = leftPad("", locationLength);
  auto arrows = leftPad(highlightArrows(location.end - location.start), location.start);
  auto line = Color::Blue("|");
  auto lineNumber = Color::Blue(location.line);

  std::println(stderr, "{} {}", padding, line);
  std::println(stderr, "{} {} {}", lineNumber, line, getNthLine(source, location.line));
  std::println(stderr, "{} {} {}", padding, line, Color::Red(arrows));
  std::println(stderr, "{} {}", Color::Yellow("ERROR:"), message);
}
