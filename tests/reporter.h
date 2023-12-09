#pragma once

#include "evaluation/evaluator.h"
#include "parsing/parser.h"
#include "parsing/scanner.h"
#include <variant>

namespace logic {

using Error = std::variant<ParserError, ScannerError, EvaluatorError>;

auto report(const Error&) -> std::string;

}
