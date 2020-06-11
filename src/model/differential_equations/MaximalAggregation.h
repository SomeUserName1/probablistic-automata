#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H

#include "../ReductionMethodInterface.h"
#include <vector>

class MaximalAggregation : public ReductionMethodInterface {
public:
  ~MaximalAggregation() override;

  [[nodiscard]] std::string get_name() const override;

  std::shared_ptr<RepresentationInterface>
  reduce(std::shared_ptr<RepresentationInterface> &ptr) const override;
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
