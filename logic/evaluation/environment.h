#pragma once

#include <cstddef>
#include <bitset>
#include <map>
#include <vector>
#include <set>

namespace logic {

class Environment {

public:
  static constexpr auto MAX_VARIABLES = 26;

private:
  std::vector<std::bitset<MAX_VARIABLES>> mData;
  std::set<std::string_view> mVariables;
  std::map<std::string, bool> mAssignedVariables;

public:

  auto define(std::string_view) -> bool;
  auto assign(std::string_view, bool value) -> void;

  auto read(std::string_view) const -> std::vector<bool>;

  auto resetDefaultValues() -> void;

  auto initializeDefaultValues() -> void;

  constexpr auto isVariableDefined(std::string_view variable) const -> bool {
    return mVariables.contains(variable);
  }

  constexpr auto isVariableAssigned(std::string_view variable) const -> bool {
    return mAssignedVariables.contains(std::string(variable));
  }

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
  constexpr auto totalVariablesAssigned() -> size_t {
    return mAssignedVariables.size();
  }

  constexpr auto definedVariables() const -> const std::set<std::string_view>& {
    return mVariables;
  }
};

}
