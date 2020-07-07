#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATION_H

#include "../RepresentationInterface.h"
#include "Term.h"

class DifferentialEquation : public Equation {
private:
  std::vector<Term> rhs std::vector<Term> lhs

      public : DifferentialEquation() = default;
  ~DifferentialEquation() override;

  [[nodiscard]] auto pretty_print() const -> std::string override {
    return nullptr;
  }

  [[nodiscard]] auto
  equivalent(const std::shared_ptr<RepresentationInterface> &other) const
      -> bool override {
    return false;
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATION_H
