#include <fmt/core.h>

#include "reporter.h"
#include "logic/utils/overloaded.h"

auto logic::report(const Error& e) -> std::string {
  return std::visit(overloaded {
    [](const ScannerError& e) -> std::string {
      return e.accept(overloaded {
        [](const ScannerError::UnexpectedKeyword &e) {
          return fmt::format("Unexpected keyword `{}`", e.keyword);
        },
        [](const ScannerError::InvalidVariableName &e) {
          return fmt::format("Invalid Variable name`{}`", e.name);
        },
        [](const ScannerError::InvalidKeywordFormat &e) {
          return fmt::format("Invalid Keyword format: `{}` must be capitalized.", e.keyword);
        },
        [](const ScannerError::UnexpectedCharacter &e) {
          return fmt::format("Unexpected character `{}`", e.character);
        }}
      );
    },
    [](const ParserError& e) -> std::string {
      return e.accept(overloaded {
        [](const ParserError::UnexpectedToken& e) {
          return fmt::format("Unexpected token {}, expected {}", e.got.lexeme, tokenTypeToString(e.expected));
        },
        [](const ParserError::ExpectedSentence& e) {
          return fmt::format("Expected sentence");
        },
      });
    },
    [](const EvaluatorError& e) -> std::string {
      return "";
    },
  }, e);
}
