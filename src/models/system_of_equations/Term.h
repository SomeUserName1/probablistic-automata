#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_TERM_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_TERM_H

#include <vector>
#include <sstream>
#include <cstring>

#include "../../util/DefsConstants.h"
#include "../../util/ParseUtils.h"
#include "../RepresentationInterface.h"

// TODO How to store non-linearities/functions as factors
// TODO how to store derivatives, powers, ... as "words"
// TODO currently only 1 * x1_2 x2 x3_3 are repr'able;
// TODO: Fancy: impl. Function, Constant, Deriv, pow and expr Term as

class Term : public RepresentationInterface {
private:
  unsigned long int factor; // FIXME Here functions & constants are possible
  std::vector<uint> word; // FIXME Powers, Derivatives, sin, cos, tan, e

public:
  Term(unsigned long int mFactor, std::vector<uint> mWord)
      : factor(mFactor), word(std::move(mWord)) {}
  ~Term() override;

  [[nodiscard]] auto get_factor() const -> unsigned long int {
    return this->factor;
  }

  [[nodiscard]] auto get_word() const -> std::vector<uint> {
    return this->word;
  }

  [[nodiscard]] auto pretty_print() const -> std::string override {
    std::stringstream stringstream;
    stringstream.precision(PRINT_PRECISION);
    stringstream << std::fixed << this->factor << " ";
    for (size_t i = 0; i < word.size(); i++) {
      if (word[i] > 0) {
        stringstream << "(x" << i << ")_" << word[i] << " ";
      }
    }
    return stringstream.str();
  }

  [[nodiscard]] auto
  equivalent(const std::shared_ptr<RepresentationInterface> &other) const
  -> bool override {
    auto oTerm = static_pointer_cast<Term>(other);
    if (this->factor != oTerm->get_factor() ||
        this->word.size() != oTerm->get_word().size()) {
      return false;
    }
    for (size_t i = 0; i < word.size(); i++) {
      if (word[i] != oTerm->get_word()[i]) {
        return false;
      }
    }
    return true;
  }

  static inline auto extract_term(const std::vector<std::string> &pMapping, std::string &input)
  -> std::shared_ptr<Term> {

    bool seenPrevEntity = false;
    std::string prevEntityStr;
    unsigned int prevEntityInt = 0;
    unsigned char next = 0;
    unsigned long int factor = 1;
    std::vector<unsigned int> word = {};

    for (unsigned int i = 0; i < pMapping.size(); i++) {
      word.push_back(0);
    }

    while (!input.empty()) {
      next = static_cast<unsigned char>(input[0]);
      if ((std::isdigit(next) != 0) && !seenPrevEntity) {
        factor = extract_number<unsigned long int>(input);
      } else if ((std::isupper(next) != 0)) {
        if (seenPrevEntity) {
          word[prevEntityInt] = 1;
        }
        seenPrevEntity = true;
        prevEntityStr = extract_entity_name(input);
        prevEntityInt = static_cast<unsigned int>(std::distance(
            pMapping.begin(),
            std::find(pMapping.begin(), pMapping.end(), prevEntityStr)));
      } else if ((std::isdigit(next) != 0) && seenPrevEntity) {
        seenPrevEntity = false;
        word[prevEntityInt] = extract_number<unsigned int>(input);
      }
      if (seenPrevEntity && input.empty()) {
        seenPrevEntity = false;
        word[prevEntityInt] = 1;
      }
    }
    return std::make_shared<Term>(factor, word);
  }

  static inline auto extract_terms(const std::vector<std::string> &pMapping, std::string &input)
  -> std::vector<std::shared_ptr<Term>> {
    std::vector<std::shared_ptr<Term>> result = {};
    std::string term;
    size_t pos = 0;
    size_t prev = 0;
    while (pos != std::string::npos) {
      pos = input.find('+', prev);
      term = input.substr(prev, pos - prev);
      result.emplace_back(extract_term(pMapping, term));
      prev = pos + strlen("+");
    }
    return result;
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_TERM_H
