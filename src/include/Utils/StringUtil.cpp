#include "Utils/StringUtil.h"

#include <algorithm>
#include <sstream>

namespace Casper {

bool StringUtil::startsWith(const std::string& str, const std::string& cmp) {
  return str.compare(0, cmp.length(), cmp) == 0;
}

std::vector<std::string> StringUtil::splitString(std::string str,
                                                 std::string delim) {
  std::vector<std::string> result;
  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, delim[0])) {
    result.push_back(token);
  }

  return result;
}

std::string StringUtil::toLower(std::string& str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

}  // namespace Casper
