#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEM_H

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "../../util/DefsConstants.h"
#include "../../util/FloatingPointCompare.h"
#include "../../util/ParseUtils.h"
#include "../RepresentationInterface.h"

class RewriteSystem : public RepresentationInterface {
public:
  class Term;
  class Rule;

private:
  std::vector<std::string> mapping;
  std::vector<std::vector<unsigned int>> speciesList;
  std::vector<std::shared_ptr<Rule>> rules;

public:
  RewriteSystem() = default;
  RewriteSystem(std::vector<std::string> mMapping,
                std::vector<std::vector<unsigned int>> mSpeciesList,
                std::vector<std::shared_ptr<Rule>> mRules)
      : mapping(std::move(mMapping)), speciesList(std::move(mSpeciesList)),
        rules(std::move(mRules)) {}

  ~RewriteSystem() override;

  [[nodiscard]] auto get_mapping() const noexcept -> const std::vector<std::string>& {
    return this->mapping;
  }

  [[nodiscard]] auto get_species_list() const noexcept
      -> const std::vector<std::vector<unsigned int>>& {
    return this->speciesList;
  }

  [[nodiscard]] auto get_rules() const noexcept
      -> const std::vector<std::shared_ptr<Rule>>& {
    return this->rules;
  }

  [[nodiscard]] auto pretty_print() const -> std::string override {
    std::stringstream stringstream;
    stringstream << "Alphabet:\n";
    for (const auto &elem : mapping) {
      stringstream << "\t " << elem << std::endl;
    }
    size_t i = 0;
    stringstream << "\n Species:\n";
    for (const auto &elem : this->speciesList) {
      for (const auto& digit : elem) {
        if (digit > 0) {
          stringstream << mapping[i];
          if (digit > 1) {
            stringstream << digit;
          }
        }
        i++;
      }
      i = 0;
      stringstream << std::endl;
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
    bool found;
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
    unsigned int species;

  public:
    Term(unsigned int mFactor, unsigned int mSpecies)
        : factor(mFactor), species(mSpecies) {}
    ~Term() override;

    [[nodiscard]] auto get_factor() const -> unsigned int {
      return this->factor;
    }

    auto decrement_factor() -> void { this->factor--; }

    [[nodiscard]] auto get_species() const -> unsigned int {
      return this->species;
    }

    void add_factor(unsigned int addition) { this->factor += addition; }

    [[nodiscard]] auto pretty_print() const -> std::string override {
      std::stringstream stringstream;
      stringstream.precision(PRINT_PRECISION);
      stringstream << std::fixed << this->factor << "(x" << this->species << ")"
                   << " ";
      return stringstream.str();
    }

    [[nodiscard]] auto
    equivalent(const std::shared_ptr<RepresentationInterface> &other) const
        -> bool override {
      auto oTerm = static_pointer_cast<Term>(other);
      return (this->factor == oTerm->get_factor() &&
              this->species == oTerm->get_species());
    }
  };

  class Rule : public RepresentationInterface {
  private:
    double rate;
    std::vector<std::shared_ptr<Term>> lhs;
    std::vector<std::shared_ptr<Term>> rhs;

  public:
    Rule(double mRate, std::vector<std::shared_ptr<Term>> mLhs,
         std::vector<std::shared_ptr<Term>> mRhs)
        : rate(mRate), lhs(std::move(mLhs)), rhs(std::move(mRhs)) {}
    ~Rule() override;

    [[nodiscard]] auto get_rate() const -> double { return this->rate; }

    [[nodiscard]] auto get_lhs() const -> const std::vector<std::shared_ptr<Term>>& {
      return this->lhs;
    }

    [[nodiscard]] auto get_rhs() const -> const std::vector<std::shared_ptr<Term>>& {
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
      bool found;
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
