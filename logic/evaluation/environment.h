#pragma once

#include <cstddef>
#include <bitset>
#include <map>
#include <vector>
#include <set>

namespace logic {

class Environment {

public:
  static constexpr auto MAX_VARIABLES = 16;

private:
  std::vector<std::bitset<MAX_VARIABLES>> mData;
  std::set<std::string_view> mVariables;

public:

  auto define(std::string_view) -> bool;
  auto read(std::string_view) const -> std::vector<bool>;

  constexpr auto createBoolean(bool value) const -> std::vector<bool> {
    auto size = 1 << mVariables.size();
    std::vector<bool> result(size);

    for (auto i=0; i< 1 << size; i++) {
      result[i] = value;
    }
    return result;
  }

  constexpr auto totalVariablesDefined() -> size_t {
    return mVariables.size();
  }
};

}
