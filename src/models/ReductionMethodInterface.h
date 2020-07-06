#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H

#include "RepresentationInterface.h"
#include <memory>

class ReductionMethodInterface {
public:
  virtual ~ReductionMethodInterface();

  [[nodiscard]] virtual inline auto get_name() const -> std::string = 0;

  [[nodiscard]] virtual inline auto
  reduce(const std::shared_ptr<RepresentationInterface> &)
      -> std::shared_ptr<RepresentationInterface> = 0;
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H
