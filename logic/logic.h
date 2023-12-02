
#include <logic/parsing/parser.h>
#include <logic/parsing/scanner.h>
#include <logic/evaluation/evaluator.h>

using namespace logic;

class Logic {

public:
  static auto runFile(std::string_view filename) -> void;
  static auto runREPL() -> void;

private:
  static auto run(std::string_view source) -> void;
  static auto report(const EvaluatorError&) -> void;
  static auto report(const ScannerError&) -> void;
  static auto report(const ParserError&) -> void;


};