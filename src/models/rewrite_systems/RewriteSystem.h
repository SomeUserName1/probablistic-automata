#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H

#include "../../util/DefsConstants.h"
#include "../../util/FloatingPointCompare.h"
#include "../RepresentationInterface.h"
#include <memory>
#include <set>
#include <utility>
#include <vector>

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
      : mapping(mMapping), rules(std::move(mRules)) {}

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
    RewriteSystem &parent;
    std::vector<uint> word;
    unsigned long int factor;

  public:
    Term(RewriteSystem &mParent, std::vector<uint> mWord,
         unsigned long int mFactor)
        : parent(mParent), word(std::move(mWord)), factor(mFactor) {}
    ~Term() override;

    [[nodiscard]] auto get_factor() const -> unsigned int {
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
          stringstream << parent.mapping[i] << "_" << word[i] << " ";
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
  };

  class Rule : public RepresentationInterface {
  private:
    RewriteSystem &parent;
    double rate;
    unsigned long long int mcoeff;
    std::vector<std::shared_ptr<RewriteSystem::Term>> lhs;
    std::vector<std::shared_ptr<RewriteSystem::Term>> rhs;

  public:
    Rule(RewriteSystem &mParent, double mRate,
         std::vector<std::shared_ptr<RewriteSystem::Term>> mLhs,
         std::vector<std::shared_ptr<RewriteSystem::Term>> mRhs)
        : parent(mParent), rate(mRate), mcoeff(0), lhs(std::move(mLhs)),
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
