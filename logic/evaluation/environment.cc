#include "logic/evaluation/environment.h"
#include "logic/utils/macros.h"

using namespace logic;

auto Environment::define(std::string_view variableName) -> bool {
  if (mVariables.contains(variableName)) return true;
  if (mVariables.size() + 1 > MAX_VARIABLES) {
    return false;
  }

  mVariables.insert(variableName);
  initializeDefaultValues();
  return true;
}

auto Environment::read(std::string_view variableName) const -> std::vector<bool> {
  auto variableNameStr = std::string(variableName);
  if (mAssignedVariables.contains(variableNameStr)) {
    return createBoolean(mAssignedVariables.at(variableNameStr));
  }

  ASSERT(mVariables.contains(variableName));

  auto result = std::vector<bool> {};

  auto offset = mVariables.size() - 1;
  for (const auto& var : mVariables) {
    if (variableName == var) { break; }
    offset -= 1;
  }

  for (const auto& data : mData) {
    result.push_back(data.test(offset));
  }

  return result;
}

auto Environment::assign(std::string_view variableName, bool value) -> void {
  if (mVariables.contains(variableName)) {
    mVariables.erase(variableName);
    initializeDefaultValues();
  }

  mAssignedVariables[std::string(variableName)] = value;
}

auto Environment::initializeDefaultValues() -> void {
  mData.clear();

  for (auto i = (1 << mVariables.size()) - 1; i >= 0; i--) {
    mData.push_back(i);
  }
}

auto Environment::resetDefaultValues() -> void {
  mData.clear();
  mVariables.clear();
}

