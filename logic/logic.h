
#include <logic/parsing/parser.h>
#include <logic/parsing/scanner.h>
#include <logic/evaluation/evaluator.h>

using namespace logic;

class Logic {

public:
  static auto runFile(std::string_view filename) -> void;
  static auto runREPL() -> void;

private:
  static auto run(std::string_view source, Environment& env) -> void;
  static auto report(const EvaluatorError&, std::string_view) -> void;
  static auto report(const ScannerError&, std::string_view) -> void;
  static auto report(const ParserError&, std::string_view) -> void;
  static auto reportInternal(std::string_view message, SourceLocation location, std::string_view source) -> void;
};
