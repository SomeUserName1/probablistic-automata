#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H

#include "WeightedAutomaton.h"

template <Matrix M>
static auto lift_wa(std::shared_ptr<WeightedAutomaton<M>> wa,
                    const std::vector<long> &states,
                    const std::vector<uint> &duplications)
    -> std::shared_ptr<WeightedAutomaton<M>> {
  // calculate new matrix size
  size_t matSize = wa->get_states();
  for (unsigned int duplication : duplications) {
    matSize += duplication;
  }
  /*
   *  Cases not to be considered:
   *  - non-initial, non-final, no incomming => unreachable
   *  - non-initial, final, no incomming => unreachable
   *  - non-initial, non-final no outgoing => dead-end
   *  - initial, no outgoing => dead-end
   *  => treat final as outgoing, initial as incomming
   *
   * Cases:
   *  - non-initial, non-final, incoming, outgoing
   *  ALWAYS div incomming by #dups
   */
  // Alpha
  auto liftedAlpha = std::make_shared<M>(1, matSize);
  long j = 0;
  long stateIndex = 0;
  double newIncomingWeight = -1.0;
  for (long i = 0; i < wa->get_states(); i++) {
    auto stateIt = std::find(states.begin(), states.end(), i);
    if (stateIt != states.end()) {
      stateIndex = std::distance(states.begin(), stateIt);
      newIncomingWeight = wa->get_alpha()->coeff(0, i) /
                          (duplications[static_cast<size_t>(stateIndex)] + 1);
      for (unsigned int k = 0;
           k < duplications[static_cast<size_t>(stateIndex)] + 1; k++) {
        liftedAlpha->coeffRef(0, j + k) = newIncomingWeight;
      }
      j += duplications[static_cast<size_t>(stateIndex)];
    } else {
      liftedAlpha->coeffRef(0, j) = wa->get_alpha()->coeff(0, i);
    }
    j++;
  }

  // eta
  auto liftedEta = std::make_shared<M>(matSize, 1);
  j = 0;
  for (long i = 0; i < wa->get_states(); i++) {
    auto stateIt = std::find(states.begin(), states.end(), i);
    if (stateIt != states.end()) {
      stateIndex = std::distance(states.begin(), stateIt);
      for (unsigned int k = 0;
           k < duplications[static_cast<size_t>(stateIndex)] + 1; k++) {
        liftedEta->coeffRef(j + k, 0) = wa->get_eta()->coeff(i, 0);
      }
      j += duplications[static_cast<size_t>(stateIndex)];
    } else {
      liftedEta->coeffRef(j, 0) = wa->get_eta()->coeff(i, 0);
    }
    j++;
  }

  // mu
  std::vector<std::shared_ptr<M>> liftedMus = {};
  std::shared_ptr<M> liftedMu = nullptr;
  j = 0;
  long l = 0;
  long stateIndexK = 0;
  long stateIndexI = 0;
  newIncomingWeight = -1;
  for (const auto &mu : wa->get_mu()) {
    liftedMu = std::make_shared<M>(matSize, matSize);
    j = 0;
    l = 0;
    for (long i = 0; i < wa->get_states(); i++) {
      l = 0;
      auto stateItI = std::find(states.begin(), states.end(), i);
      for (long k = 0; k < wa->get_states(); k++) {
        auto stateItK = std::find(states.begin(), states.end(), k);
        if (stateItI != states.end()) {
          stateIndexI = std::distance(states.begin(), stateItI);
          for (unsigned int n = 0;
               n < duplications[static_cast<size_t>(stateIndexI)] + 1; n++) {
            if (stateItK != states.end()) {
              stateIndexK = std::distance(states.begin(), stateItK);
              for (unsigned int m = 0;
                   m < duplications[static_cast<size_t>(stateIndexK)] + 1;
                   m++) {
                liftedMu->coeffRef(j + n, l + m) = mu->coeff(i, k);
              }
            } else {
              liftedMu->coeffRef(j + n, l) = mu->coeff(i, k);
            }
          }
        } else if (stateItK != states.end()) {
          stateIndexK = std::distance(states.begin(), stateItK);
          newIncomingWeight =
              mu->coeff(i, k) /
              (duplications[static_cast<size_t>(stateIndexK)] + 1);
          for (unsigned int n = 0;
               n < duplications[static_cast<size_t>(stateIndexK)] + 1; n++) {
            liftedMu->coeffRef(j, l + n) = newIncomingWeight;
          }
        } else {
          liftedMu->coeffRef(j, l) = mu->coeff(i, k);
        }
        if (stateItK != states.end()) {
          l += duplications[static_cast<size_t>(stateIndexK)];
        }
        l++;
      }
      if (stateItI != states.end()) {
        j += duplications[static_cast<size_t>(stateIndexI)];
      }
      j++;
    }
    liftedMus.push_back(liftedMu);
  }

  return std::make_shared<WeightedAutomaton<M>>(
      matSize, wa->get_number_input_characters(), liftedAlpha, liftedMus,
      liftedEta);
}
#endif // STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H
