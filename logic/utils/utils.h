
#pragma once

#include <string>
#include <string_view>

auto levenshteinDistance(std::string_view word1, std::string_view word2) -> int;
auto stringToUpper(std::string_view strToConvert) -> std::string;

