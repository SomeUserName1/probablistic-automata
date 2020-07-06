#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H

#include "DefsConstants.h"
#include <cstring>
#include <string>

enum TrimType { TrimWhiteSpace = 0, TrimNewLines = 1, DontTrim = 2 };

inline static auto get_next_line(std::string &str, const char *delim,
                                 TrimType trimType) noexcept -> std::string {
  std::string line;
  std::size_t pos = str.find(delim);
  if (pos != std::string::npos) {
    pos = pos + strlen(delim);
  } else {
    pos = line.size() - 1;
  }
  line = str.substr(0, pos);
  str.erase(0, pos);
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
static inline auto extract_number(std::string &vector) -> T {
  bool prevDigit = false;
  size_t firstDigit = 0;
  unsigned char current = 0;

  for (size_t i = 0; i < vector.size(); i++) {
    current = static_cast<unsigned char>(vector[i]);
    if (((std::isdigit(current) != 0) || vector[i] == '-' || vector[i] == '+' ||
         vector[i] == '.') &&
        !prevDigit) {
      prevDigit = true;
      firstDigit = i;
      if (i == vector.size() - 1) {
        T result = static_cast<T>(std::stod(vector.substr(i, 1)));
        vector.erase(0, i + 1);
        return result;
      }
    } else if (!((std::isdigit(current) != 0) || vector[i] == 'e' ||
                 vector[i] == '-' || vector[i] == '+' || vector[i] == '.') &&
               prevDigit) {
      T result =
          static_cast<T>(std::stod(vector.substr(firstDigit, i - firstDigit)));
      vector.erase(0, i);
      return result;
    } else if (vector.size() - 1 == i && prevDigit) {
      T result = static_cast<T>(
          std::stod(vector.substr(firstDigit, i - firstDigit + 1)));
      vector.erase(0, i + 1);
      return result;
    }
  }
  throw std::invalid_argument("No numbers found in the input!");
}

static inline auto extract_atomic_name(std::string &line) -> std::string {
  bool prevCapital = false;
  size_t capital = 0;
  unsigned char current = 0;

  for (size_t i = 0; i < line.size(); i++) {
    current = static_cast<unsigned char>(line[i]);
    if (std::isupper(current) != 0 && !prevCapital) {
      prevCapital = true;
      capital = i;
      if (i == line.size() - 1) {
        std::string result = line.substr(i, 1);
        line.erase(0, i + 1);
        return result;
      }
    } else if (std::islower(current) == 0 && prevCapital) {
      std::string result = line.substr(capital, i - capital);
      line.erase(0, i);
      return result;
    } else if (line.size() - 1 == i && prevCapital) {
      std::string result = line.substr(capital, i - capital + 1);
      line.erase(0, i + 1);
      return result;
    }
  }
  line.erase(0, line.size());
  return "";
}

static inline auto extract_species_name(std::string &line) -> std::string {
  bool firstCap = false;
  size_t capital = 0;
  unsigned char current = 0;

  line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

  for (size_t i = 0; i < line.size(); i++) {
    current = static_cast<unsigned char>(line[i]);
    if (std::isupper(current) != 0 && !firstCap) {
      firstCap = true;
      capital = i;
      if (i == line.size() - 1) {
        std::string result = line.substr(i, 1);
        line.erase(0, i + 1);
        return result;
      }
    } else if (std::isalnum(current) == 0 && firstCap) {
      std::string result = line.substr(capital, i - capital);
      line.erase(0, i);
      return result;
    } else if (line.size() - 1 == i && firstCap) {
      std::string result = line.substr(capital, i - capital + 1);
      line.erase(0, i + 1);
      return result;
    }
  }
  line.erase(0, line.size());
  return "";
}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_PARSEUTILS_H
