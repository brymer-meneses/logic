
#include "reporter.h"
#include "logic/utils/overloaded.h"

auto logic::report(const Error& e) -> std::string {
  return std::visit(overloaded {
    [](const ScannerError& e) -> std::string {
      return e.accept(overloaded {
        [](const ScannerError::UnexpectedKeyword &e) {
          return std::format("Unexpected keyword `{}`", e.keyword);
        },
        [](const ScannerError::InvalidVariableName &e) {
          return std::format("Invalid Variable name`{}`", e.name);
        },
        [](const ScannerError::UnexpectedCharacter &e) {
          return std::format("Unexpected character `{}`", e.character);
        }}
      );
    },
    [](const ParserError& e) -> std::string {
      return e.accept(overloaded {
        [](const ParserError::ExpectedToken& e) {
          return std::format("Unexpected token {}, expected {}", e.got.lexeme, tokenTypeToString(e.expected));
        },
        [](const ParserError::ExpectedSentence& e) {
          return std::format("Expected sentence");
        }});
    },
    [](const EvaluatorError& e) -> std::string {
      return "";
    },
  }, e);
}
