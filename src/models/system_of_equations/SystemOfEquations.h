#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_SYSTEMOFEQUATIONS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_SYSTEMOFEQUATIONS_H

#include <vector>

#include "../RepresentationInterface.h"
#include "Equation.h"

class SystemOfEquations : public RepresentationInterface {
private:
  std::vector<std::shared_ptr<Equation>> equations;

public:
  explicit SystemOfEquations(std::vector<std::shared_ptr<Equation>> eqs)
      : equations(std::move(eqs)) {}

  ~SystemOfEquations() override;

  [[nodiscard]] auto pretty_print() const -> std::string override {
    return nullptr;
  }

  [[nodiscard]] auto
  equivalent(const std::shared_ptr<RepresentationInterface> &other) const
      -> bool override {
    return false;
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_SYSTEMOFEQUATIONS_H
