#include "logic/environment.h"
#include "logic/utils.h"
#include <print>

using namespace logic;

Environment::Environment(uint8_t numVariables) : mNumVariables(numVariables) {
  auto maxNum = (1 << mNumVariables) - 1;
  for (auto i=maxNum; i >= 0; i--) {
    mData.push_back(i);
  }

  ASSERT(mData.size() == 1 << mNumVariables);
};

auto Environment::defineVariable(std::string_view variableName) -> void {
  static uint8_t currentIndex = mNumVariables-1;
  mOffsets[variableName] = currentIndex--;
}

auto Environment::readVariable(std::string_view variableName) -> std::vector<bool> {
  ASSERT(mOffsets.contains(variableName),
         "INTERNAL ERROR: Tried to read variable `{}` is not defined",
         variableName);

  auto result = std::vector<bool> {};
  auto offset = mOffsets[variableName];
  for (const auto& data : mData) {
    result.push_back(data.test(offset));
  }

  ASSERT(result.size() == 1 << mNumVariables);
  return result;
}
