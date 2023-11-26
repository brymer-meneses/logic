#include <iostream>

#include "logic/parsing/sentence.h"
#include "logic/evaluation/value.h"

namespace logic {

auto operator<<(std::ostream& stream, const Sentence& sentence) -> std::ostream&;

auto operator<<(std::ostream& stream, const Value& value) -> std::ostream&;

}
