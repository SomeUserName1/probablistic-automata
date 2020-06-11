#include "MaximalAggregation.h"
#include "DifferentialEquation.h"

MaximalAggregation::~MaximalAggregation() = default;

std::string MaximalAggregation::get_name() const {
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
