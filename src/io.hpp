#pragma once

#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace twoopt::detail {

// All integers of a text stream; '#' starts a comment that runs to the end of the line.
inline std::vector<long long> readIntegers(std::istream& in) {
  std::vector<long long> values;
  std::string line;
  while (std::getline(in, line)) {
    line = line.substr(0, line.find('#'));
    std::istringstream tokens(line);
    std::string token;
    while (tokens >> token) {
      std::size_t used = 0;
      values.push_back(std::stoll(token, &used));
      if (used != token.size()) throw std::runtime_error("not an integer: " + token);
    }
  }
  return values;
}

}  // namespace twoopt::detail
