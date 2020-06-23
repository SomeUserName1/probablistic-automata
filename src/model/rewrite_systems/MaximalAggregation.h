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

  void largest_equivalence();
  void init();
  void backward_prepartitioning();
  void split();
  void compute_forward_reduction();
  void compute_backward_reduction();
  void apply_reduction();
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
