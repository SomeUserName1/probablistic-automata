#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H

#include "../util/NotImplementedException.h"
#include <memory>

// Used to avoid a cyclic dependency: ModelInterface to ReductionMethodInterface
// to ModelInterface
class RepresentationInterface {
public:
  virtual ~RepresentationInterface();

  [[nodiscard]] virtual auto pretty_print() const -> std::string = 0;

  [[nodiscard]] virtual auto equivalent(const std::shared_ptr<RepresentationInterface> &other) const -> bool = 0;
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H
