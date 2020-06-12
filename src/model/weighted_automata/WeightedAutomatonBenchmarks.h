#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H

#include "WeightedAutomaton.h"

template <Matrix M>
static std::shared_ptr<WeightedAutomaton<M>>
lift_wa(std::shared_ptr<WeightedAutomaton<M>> wa, std::vector<long> states,
        std::vector<uint> duplications) {
  // alloc
  for (long s = 0; s < states.size(); s++) {

  }
}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H
