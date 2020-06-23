//
// Created by someusername on 5/28/20.
//

#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATION_H

#include "../RepresentationInterface.h"
class DifferentialEquation : public RepresentationInterface {
public:
  DifferentialEquation() = default;
  ~DifferentialEquation() override;

  [[nodiscard]] auto pretty_print() const -> std::string override { return nullptr; }

  [[nodiscard]] auto equivalent(
      const std::shared_ptr<RepresentationInterface> &other) const -> bool override {
    return false;
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATION_H
