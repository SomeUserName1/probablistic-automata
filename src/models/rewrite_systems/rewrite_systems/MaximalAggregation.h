#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H

#include "../ReductionMethodInterface.h"
#include "../differential_equations/DifferentialEquation.h"
#include <vector>

class MaximalAggregation : public ReductionMethodInterface {
public:
  ~MaximalAggregation() override;

  [[nodiscard]] std::string MaximalAggregation::get_name() const {
    return "Maximal Aggregation";
  }

  std::shared_ptr<RepresentationInterface> MaximalAggregation::reduce(
      std::shared_ptr<RepresentationInterface> &ptr) const {
    std::shared_ptr<DifferentialEquation> diffEq =
        std::dynamic_pointer_cast<DifferentialEquation>(ptr);
    std::shared_ptr<ReactionNetwork> reactionNet =
        ReactionNetwork::convertDifferentialEquation(
            (diffEq);) return std::shared_ptr<RepresentationInterface>();
  }


  void largest_equivalence();
  void init();
  void backward_prepartitioning();
  void split();
  void compute_forward_reduction();
  void compute_backward_reduction();
  void apply_reduction();
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
