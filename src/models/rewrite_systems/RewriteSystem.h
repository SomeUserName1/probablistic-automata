#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "../../util/DefsConstants.h"
#include "../../util/ParseUtils.h"
#include "../../util/FloatingPointCompare.h"
#include "../RepresentationInterface.h"

class RewriteSystem : public RepresentationInterface {
public:
  class Term;
  class Rule;

private:
  std::vector<std::string> mapping;
  std::vector<std::shared_ptr<Rule>> rules;

public:
  RewriteSystem() = default;
  RewriteSystem(std::vector<std::string> mMapping,
                std::vector<std::shared_ptr<Rule>> mRules)
      : mapping(std::move(mMapping)), rules(std::move(mRules)) {}

  ~RewriteSystem() override;

  [[nodiscard]] auto get_mapping() const noexcept -> std::vector<std::string> {
    return this->mapping;
  }

  [[nodiscard]] auto get_rules() const noexcept
      -> std::vector<std::shared_ptr<Rule>> {
    return this->rules;
  }

  [[nodiscard]] auto pretty_print() const -> std::string override {
    std::stringstream stringstream;
    stringstream << "Alphabet:\n";
    for (const auto &elem : mapping) {
      stringstream << "\t " << elem << std::endl;
    }
    stringstream << "\n Rules:\n";
    for (const auto &rule : this->rules) {
      stringstream << "\t" << rule->pretty_print();
    }
    return stringstream.str();
  }

  // FIXME Dummy: Only checks if they are exactly the same, not in terms of
  // dynamic semantics
  [[nodiscard]] auto
  equivalent(const std::shared_ptr<RepresentationInterface> &other) const
      -> bool override {
    auto oRewriteSystem = static_pointer_cast<RewriteSystem>(other);
    if (this->mapping.size() != oRewriteSystem->get_mapping().size() ||
        this->rules.size() != oRewriteSystem->get_rules().size()) {
      return false;
    }
    bool found = false;
    for (const auto &rule : this->rules) {
      found = false;
      for (const auto &oRule : oRewriteSystem->get_rules()) {
        if (rule->equivalent(oRule)) {
          found = true;
        }
      }
      if (!found) {
        return false;
      }
    }
    return true;
  }

  class Term : public RepresentationInterface {
  private:
    unsigned long int factor; // FIXME Here functions & constants are possible
    std::vector<uint> word; // FIXME Powers, Derivatives, mutliplicities

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

  class Rule : public RepresentationInterface {
  private:
    double rate;
    unsigned long long int mcoeff;
    std::vector<std::shared_ptr<Term>> lhs;
    std::vector<std::shared_ptr<Term>> rhs;

  public:
    Rule(double mRate,
         std::vector<std::shared_ptr<Term>> mLhs,
         std::vector<std::shared_ptr<Term>> mRhs)
        : rate(mRate), mcoeff(0), lhs(std::move(mLhs)),
          rhs(std::move(mRhs)) {}
    ~Rule() override;

    [[nodiscard]] auto get_rate() const -> double { return this->rate; }

    [[nodiscard]] auto get_mcoeff() -> unsigned long long int {
      if (this->mcoeff == 0) {
        this->set_mcoeff();
      }
      return this->mcoeff;
    }

    void set_mcoeff() {
      unsigned int upper = 0;
      unsigned long long int lower = 1;
      for (const auto &term : this->lhs) {
        upper += term->get_factor();
        lower *= FACTORIALS[term->get_factor()];
      }
      this->mcoeff = FACTORIALS[upper] / lower;
    }

    [[nodiscard]] auto get_lhs() const
        -> std::vector<std::shared_ptr<Term>> {
      return this->lhs;
    }

    [[nodiscard]] auto get_rhs() const
        -> std::vector<std::shared_ptr<Term>> {
      return this->rhs;
    }

    [[nodiscard]] auto pretty_print() const -> std::string override {
      std::stringstream stringstream;
      stringstream.precision(PRINT_PRECISION);

      stringstream << lhs[0]->pretty_print();
      for (size_t i = 1; i < lhs.size(); i++) {
        stringstream << " + " << lhs[i]->pretty_print();
      }

      stringstream << "->";

      stringstream << rhs[0]->pretty_print();
      for (size_t i = 1; i < rhs.size(); i++) {
        stringstream << " + " << rhs[i]->pretty_print();
      }
      stringstream << "\t, " << std::fixed << rate << std::endl;
      return stringstream.str();
    }

    // FIXME Dummy: Only checks if they are exactly the same, not in terms of
    // dynamic semantics
    [[nodiscard]] auto
    equivalent(const std::shared_ptr<RepresentationInterface> &other) const
        -> bool override {
      auto oRule = static_pointer_cast<Rule>(other);
      if (oRule == nullptr) {
        return false;
      }
      if (!floating_point_compare(this->rate - oRule->get_rate(), 0.0) ||
          this->lhs.size() != oRule->get_lhs().size() ||
          this->rhs.size() != oRule->get_rhs().size()) {
        return false;
      }
      bool found = false;
      for (const auto &term : this->lhs) {
        found = false;
        for (const auto &oTerm : oRule->get_lhs()) {
          if ((*term).equivalent(oTerm)) {
            found = true;
          }
        }
        if (!found) {
          return false;
        }
      }
      for (const auto &term : this->rhs) {
        found = false;
        for (const auto &oTerm : oRule->get_rhs()) {
          if ((*term).equivalent(oTerm)) {
            found = true;
          }
        }
        if (!found) {
          return false;
        }
      }
      return true;
    }
  };
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H
