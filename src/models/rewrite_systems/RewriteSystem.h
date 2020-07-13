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

  [[nodiscard]] auto get_mapping() const noexcept
      -> const std::vector<std::string> & {
    return this->mapping;
  }

  [[nodiscard]] auto get_species_list() const noexcept
      -> const std::vector<std::vector<unsigned int>> & {
    return this->speciesList;
  }

  [[nodiscard]] auto get_rules() const noexcept
      -> const std::vector<std::shared_ptr<Rule>> & {
    return this->rules;
  }

  [[nodiscard]] auto get_name_for_species(const std::vector< unsigned int> &vec) const -> std::string {
    std::string res;
    for (size_t i = 0; i < vec.size(); i++) {
      if (vec[i] > 0) {
        res += this->mapping[i];
        if (vec[i] > 1) {
          res += std::to_string(vec[i]);
        }
      }
    }
    return res;
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
      stringstream << "\t ";
      for (const auto &digit : elem) {
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

  class Rule : public RepresentationInterface {
  private:
    double rate;
    // each term in a hand-side consists of an uint giving the factor and an
    // uint giving the index of the species stored in the species list
    std::vector<std::array<unsigned int, 2>> lhs;
    std::vector<std::array<unsigned int, 2>> rhs;

  public:
    Rule(double mRate, std::vector<std::array<unsigned int, 2>> mLhs,
         std::vector<std::array<unsigned int, 2>> mRhs)
        : rate(mRate), lhs(std::move(mLhs)), rhs(std::move(mRhs)) {}
    ~Rule() override;

    [[nodiscard]] auto get_rate() const -> double { return this->rate; }

    [[nodiscard]] auto get_lhs() const
        -> const std::vector<std::array<unsigned int, 2>> & {
      return this->lhs;
    }

    [[nodiscard]] auto get_rhs() const
        -> const std::vector<std::array<unsigned int, 2>> & {
      return this->rhs;
    }

    // FIXME
    //  use mapping n stuff
    [[nodiscard]] auto pretty_print() const -> std::string override {
      std::stringstream stringstream;
      stringstream.precision(PRINT_PRECISION);

      stringstream << lhs[0][0] << " (x" << lhs[0][1] << ")";
      for (size_t i = 1; i < lhs.size(); i++) {
        stringstream << " + " << lhs[i][0];
      }

      stringstream << "-> ";

      stringstream << rhs[0][0] << " (x" << rhs[0][1] << ")";
      for (size_t i = 1; i < rhs.size(); i++) {
        stringstream << " + " << rhs[0][0] << " (x" << rhs[0][1] << ")";
      }
      stringstream << "\t, " << std::fixed << rate << std::endl;
      return stringstream.str();
    }

    // FIXME
    //  Dummy: Only checks if they are exactly the same, not in terms of
    //  dynamic semantics
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
          if (term[0] == oTerm[0] && term[1] == oTerm[1]) {
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
          if (term[0] == oTerm[0] && term[1] == oTerm[1]) {
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
