#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H

#include "../ReductionMethodInterface.h"
#include "../rewrite_systems/RewriteSystemModel.h"

#include <vector>

class MaximalAggregation : public ReductionMethodInterface {
public:
  ~MaximalAggregation() override;

  [[nodiscard]] std::string MaximalAggregation::get_name() const {
    return "Maximal Aggregation";
  }

  std::shared_ptr<RepresentationInterface> MaximalAggregation::reduce(
      std::shared_ptr<RepresentationInterface> &ptr) const {
    std::shared_ptr<ReactionNetwork> reactionNet = static_pointer_cast<RewriteSystem>(ptr);


            return std::shared_ptr<RepresentationInterface>();
  }


   std::vector<unsigned int> largest_equivalent_parition(bool forward, RewriteSystem& system) {

  }

  // Build labels, pos vector, mcoeff, matrix M
  void init(std::vector<std::shared_ptr<RewriteSystem::Rule>>& rules) {
    for (const auto& rule : rules) {

    }
  }

  void backward_prepartitioning();
  void split();
  void compute_forward_reduction();
  void compute_backward_reduction();
  RewriteSystem apply_reduction(RewriteSystem& system, std::vector<unsigned int> partition);
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
