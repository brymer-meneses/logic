#include "logic/evaluation/environment.h"
#include "logic/utils.h"
#include <print>

using namespace logic;

auto Environment::define(std::string_view variableName) -> bool {
  if (mVariables.contains(variableName)) return true;
  if (mVariables.size() + 1 > MAX_VARIABLES) {
    return false;
  }

  mVariables.insert(variableName);

  mData.clear();
  mData.reserve(1 << mVariables.size());

  for (auto i = (1 << mVariables.size()) - 1; i >= 0; i--) {
    mData.push_back(i);
  }

  return true;
}

auto Environment::read(std::string_view variableName) const -> std::vector<bool> {
  ASSERT(mVariables.contains(variableName),
         "INTERNAL ERROR: Tried to read variable `{}` is not defined",
         variableName);

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
