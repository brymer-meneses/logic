
#include "logic/logic.h"
#include "evaluation/environment.h"
#include "logic/parsing/parser.h"
#include "logic/utils/overloaded.h"
#include "logic/utils/color.h"
#include "logic/utils/utils.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>

#include <utility>

using namespace logic;


auto Logic::run(std::string_view source, Environment& environment, std::string_view filename) -> void {
  auto scanner = Scanner(source, filename);
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
    auto evaluator = Evaluator(environment);
    auto value = evaluator.evaluate(sentence);
    if (not value.has_value()) {
      Logic::report(value.error(), source);
      return;
    }

    evaluator.printEvaluation();
  }
}

auto Logic::runREPL() -> void {

  Environment environment;
  while (true) {
    std::string source;
    fmt::print(">>> ");
    
    if (not std::getline(std::cin, source)) { break; }

    run(source, environment, "REPL");
  }
}

auto Logic::runFile(std::string_view filename) -> void {
  std::ifstream file(filename);
  std::stringstream source;

  if (file.fail()) {
    fmt::println(stderr, "{}: File `{}` does not exist", Color::Blue("LOGIC"), Color::Yellow(filename));
    exit(1);
  }

  if (file.is_open()) {
    std::string line;
    while (file) {
      std::getline(file, line);
      source << line << "\n";
    }
  }

  Environment environment;
  run(source.str(), environment, filename);
}

auto Logic::report(const ParserError& e, std::string_view source) -> void {
  auto location = e.accept([](auto& error){return error.location;}) ;
  auto message = e.accept(overloaded{
      [](const ParserError::UnexpectedToken &e) {
        return fmt::format("Unexpected token {}, expected {}", e.got.lexeme,
                           tokenTypeToString(e.expected));
      },
      [](const ParserError::ExpectedSentence &e) {
        return fmt::format("Expected sentence about here.");
      }});
  reportInternal(message, location, source);
}

auto Logic::report(const ScannerError& e, std::string_view source) -> void {
  auto location = e.accept([](auto& error){return error.location;}) ;
  auto message = e.accept(overloaded {
    [](const ScannerError::UnexpectedKeyword &e) {
      return fmt::format("Unexpected keyword `{}` did you mean `{}`?", e.keyword, e.suggestion);
    },
    [](const ScannerError::InvalidKeywordFormat &e) {
      return fmt::format("The keyword `{}` is not capitalized correctly. It should be `{}`.", e.keyword, stringToUpper(e.keyword));
    },
    [](const ScannerError::InvalidVariableName &e) {
      return fmt::format("Invalid variable name `{}`. Variables must have length 1", e.name);
    },
    [](const ScannerError::UnexpectedCharacter &e) {
      return fmt::format("Unexpected character `{}`", e.character);
    }
  });

  reportInternal(message, location, source);
}

auto Logic::report(const EvaluatorError& e, std::string_view source) -> void {
  auto location = e.accept([](auto& error){return error.location;}) ;
  auto message = e.accept(overloaded{
      [](const EvaluatorError::MaximumVariablesAchieved &e) -> std::string {
        return fmt::format("Maximum variables reached, this program only supports up to {} variables.", Environment::MAX_VARIABLES);
      },
      [](const EvaluatorError::InvalidAssignment &e) -> std::string {
        return "Invalid assignment. Ensure that the left-hand side is a "
               "variable and the right-hand side is either TRUE or FALSE.";
      },
  });
  reportInternal(message, location, source);
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
    return fmt::format("{}{}", std::string(length, ' '), string);
  };
  static constexpr auto highlightArrows = [](size_t length) -> std::string {
    auto formatStr = fmt::format("{{:^^{}}}", length);
    return fmt::vformat(formatStr, fmt::make_format_args(""));
  };

  if (location.filename != "REPL")  {

  }

  auto locationLength = std::to_string(location.line).length();
  auto padding = leftPad("", locationLength);
  auto arrows = leftPad(highlightArrows(location.end - location.start), location.start);
  auto line = Color::Blue("|");
  auto lineNumber = Color::Blue(location.line);

  fmt::println(stderr, "{} {}", padding, line);
  fmt::println(stderr, "{} {} {}", lineNumber, line, getNthLine(source, location.line));
  fmt::println(stderr, "{} {} {}", padding, line, Color::Red(arrows));
  fmt::println(stderr, "{} {}", Color::Yellow("ERROR:"), message);
}
