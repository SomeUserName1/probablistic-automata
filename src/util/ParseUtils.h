#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H

#include <string>

enum TrimType { TrimWhiteSpace = 0, TrimNewLines = 1, DontTrim = 2 };

inline static auto get_next_line(std::string &str, std::string &delim,
                                 TrimType trimType) noexcept -> std::string {
  std::string line;
  std::size_t pos = str.find(delim);
  line = str.substr(0, pos + delim.size());
  str.erase(0, pos + 1);
  switch (trimType) {
    case (TrimType::TrimWhiteSpace) {
      line.erase(std::remove_if(line.begin(), line.end(), ::isspace),
                 line.end());
      break;
    }
  case (TrimType::TrimNewLines) {
      line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
      break;
    }
  case (TrimType::DontTrim) {
      break;
    }
  }
  return line;
}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H
