#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H

#include "../../util/FloatingPointCompare.h"
#include "../../util/TypeDefs.h"
#include "../RepresentationInterface.h"
#include <memory>
#include <utility>
#include <vector>

class RewriteSystem : public RepresentationInterface {
public:
  class Term;
  class Rule;

private:
  unsigned int noCharacters;
  std::vector<std::shared_ptr<Rule>> rules;

public:
  RewriteSystem() = default;
  RewriteSystem(unsigned int mNoChars,
                std::vector<std::shared_ptr<Rule>> mRules)
      : noCharacters(mNoChars), rules(std::move(mRules)) {}

  ~RewriteSystem() override;

  [[nodiscard]] auto get_no_characters() const noexcept -> unsigned int {
    return this->noCharacters;
  }

  [[nodiscard]] auto get_rules() const noexcept
      -> std::vector<std::shared_ptr<Rule>> {
    return this->rules;
  }

  [[nodiscard]] auto pretty_print() const -> std::string override {
    std::stringstream stringstream;
    stringstream << "Alphabet:\n"
                 << "\t (x0)";
    for (uint i = 1; i < this->noCharacters; i++) {
      stringstream << ", (x" << i << ")";
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
    if (this->noCharacters != oRewriteSystem->get_no_characters() ||
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
    unsigned int factor;
    std::vector<uint> word;

  public:
    Term(uint mFactor, std::vector<uint> mWord)
        : factor(mFactor), word(std::move(mWord)) {}
    ~Term() override;

    [[nodiscard]] auto get_factor() const -> int { return this->factor; }

    [[nodiscard]] get_word() const->std::vector<uint> { return this->word; }

    [[nodiscard]] auto pretty_print() const -> std::string override {
      std::stringstream stringstream;
      stringstream.precision(PRINT_PRECISION);
      stringstream << fixed << this->factor << " ";
      for (size_t i = 0; i < word.size(); i++) {
        if (word[i] > 0) {
          stringstream << "(x" << i << ")_" + word[i] + " ";
        }
      }
      return stringstream.str();
    }

    [[nodiscard]] auto
    equivalent(const std::shared_ptr<RepresentationInterface> &other) const
        -> bool override {
      auto oTerm = static_pointer_cast<Term>(other);
      if (!floating_point_compare(this->factor - oTerm->get_factor(), 0.0) ||
          this->word.size() != oTerm->get_word.size()) {
        return false;
      }
      for (int i = 0; i < word.size(); i++) {
        if (word[i] != oTerm->get_word()[i]) {
          return false;
        }
      }
      return true;
    }
  };

  class Rule : public RepresentationInterface {
  private:
    double rate;
    double mcoeff;
    std::vector<std::shared_ptr<RewriteSystem::Term>> lhs;
    std::vector<std::shared_ptr<RewriteSystem::Term>> rhs;

  public:
    Rule(double mRate, std::vector<std::shared_ptr<RewriteSystem::Term>> mLhs,
         std::vector<std::shared_ptr<RewriteSystem::Term>> mRhs)
        : rate(mRate), mcoeff(std::nan), lhs(std::move(mLhs)),
          rhs(std::move(mRhs)) {
    }
    ~Rule() override;

    [[nodiscard]] auto get_rate() const -> double { return this->rate; }

    [[nodiscard]] auto get_mcoeff() const->double {
      if (this->mcoeff == std::nan) {
        throw std::logic_error("Initialize the mcoef first!");
      }
      return this->mcoeff;
    }

    [[nodiscard]] void set_mcoeff(double multCoeff) {
      unsigned int upper = 0;
      unsigned long long int lower = 1;
      for (const auto& term : this->lhs) {
        upper += term->get_factor();
        lower *= FACTORIALS[term->get_factor()];
      }
      this->mcoeff = FACTORIALS[upper] / lower;
    }

    [[nodiscard]] auto get_lhs() const
        -> std::vector<std::shared_ptr<RewriteSystem::Term>> {
      return this->lhs;
    }

    [[nodiscard]] auto get_rhs() const
        -> std::vector<std::shared_ptr<RewriteSystem::Term>> {
      return this->rhs;
    }

    [[nodiscard]] auto pretty_print() const -> std::string override {
      std::stringstream stringstream;
      stringstream.precision(PRINT_PRECISION);

      stringstream << *(lhs[0]);
      for (size_t i = 1; i < lhs.size(); i++) {
        stringstream << " + " << *(lhs[i]);
      }

      stringstream << "-- " << fixed << rate << " -->";

      stringstream << *(rhs[0]);
      for (size_t i = 1; i < rhs.size(); i++) {
        stringstream << " + " << *(rhs[i]);
      }
      stringstream << std::endl;
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
      if (!floating_point_compare(this->rate - other->get_rate(), 0.0) ||
          this->lhs.size() != other->get_lhs().size() ||
          this->rhs.size() != other->get_rhs().size()) {
        return false;
      }
      bool found = false;
      for (const auto &term : this->lhs) {
        found = false;
        for (const auto &oTerm : other->get_lhs()) {
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
        for (const auto &oTerm : other->get_rhs()) {
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
