#pragma once

#include <cstddef>
#include <vector>
#include <bitset>
#include <map>

namespace logic {

class Environment {

public:
  static constexpr auto MAX_VARIABLES = 16;

private:
  uint8_t mNumVariables;
  std::vector<std::bitset<MAX_VARIABLES>> mData;
  std::map<std::string_view, uint8_t> mOffsets;

public:
  Environment(uint8_t numVariables);

  auto defineVariable(std::string_view) -> void;
  auto readVariable(std::string_view) -> std::vector<bool>;
};

}
