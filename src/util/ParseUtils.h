#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H

#include <string>
#include "DefsConstants.h"

enum TrimType { TrimWhiteSpace = 0, TrimNewLines = 1, DontTrim = 2 };

inline static auto get_next_line(std::string &str, const char* delim,
                                 TrimType trimType) noexcept -> std::string {
  std::string line;
  std::size_t pos = str.find(delim);
  line = str.substr(0, pos + sizeof(delim));
  str.erase(0, pos + 1);
  switch (trimType) {
  case (TrimType::TrimWhiteSpace): {
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
    break;
  }
  case (TrimType::TrimNewLines): {
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    break;
  }
  case (TrimType::DontTrim): {
    break;
  }
  }
  return line;
}

template <Arithmetic T>
static inline auto extract_one_digit(std::string &vector) -> T {
  bool prevDigit = false;
  size_t firstDigit = 0;
  for (size_t i = 0; i < vector.size(); i++) {
    if ((std::isdigit(vector[i]) || vector[i] == 'e' || vector[i] == '-' ||
         vector[i] == '+' || vector[i] == '.') &&
        !prevDigit) {
      prevDigit = true;
      firstDigit = i;
    } else if (!(std::isdigit(vector[i]) || vector[i] == 'e' ||
                 vector[i] == '-' || vector[i] == '+' || vector[i] == '.') &&
               prevDigit) {
      T result = static_cast<T>(std::stod(vector.substr(firstDigit, i)));
      vector.erase(0, i);
      return result;
    }
  }
  throw std::invalid_argument("No numbers found in the input!");
}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H
