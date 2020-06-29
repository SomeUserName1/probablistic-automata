#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H

#include "WeightedAutomaton.h"

template <Matrix M>
static inline auto lift_wa(std::shared_ptr<WeightedAutomaton<M>> wa,
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

static inline auto gen_wa_dense()
-> std::shared_ptr<WeightedAutomaton<MatDenD>> {
  uint states = 4;
  uint characters = 2;
  MatDenDPtr alpha = std::make_shared<MatDenD>(1, states);
  *alpha << 1, 0, 0, 0;
  MatDenDPtr eta = std::make_shared<MatDenD>(states, 1);
  *eta << 0, 0, 0, 1;
  MatDenDPtr mu1 = std::make_shared<MatDenD>(states, states);
  *mu1 << 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
  MatDenDPtr mu2 = std::make_shared<MatDenD>(states, states);
  *mu2 << 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0;
  std::vector<MatDenDPtr> mu = {mu1, mu2};
  return std::make_shared<WeightedAutomaton<MatDenD>>(states, characters, alpha,
                                                      mu, eta);
}

static inline auto gen_wa_hand_min_dense()
-> std::shared_ptr<WeightedAutomaton<MatDenD>> {
  uint states = 3;
  uint characters = 2;
  MatDenDPtr alpha = std::make_shared<MatDenD>(1, states);
  *alpha << 1, 0, 0;
  MatDenDPtr eta = std::make_shared<MatDenD>(states, 1);
  *eta << 0, 0, 1;
  MatDenDPtr mu1 = std::make_shared<MatDenD>(states, states);
  *mu1 << 0, 2, 0, 0, 0, 0, 0, 0, 0;
  MatDenDPtr mu2 = std::make_shared<MatDenD>(states, states);
  *mu2 << 0, 0, 0, 0, 0, 1, 0, 0, 0;
  std::vector<MatDenDPtr> mu = {mu1, mu2};
  return std::make_shared<WeightedAutomaton<MatDenD>>(states, characters, alpha,
                                                      mu, eta);
}

static inline auto gen_wa_sparse()
-> std::shared_ptr<WeightedAutomaton<MatSpD>> {
  uint states = 4;
  uint characters = 2;
  MatSpDPtr alpha = std::make_shared<MatSpD>(1, states);
  alpha->coeffRef(0, 0) = 1.0;
  MatSpDPtr eta = std::make_shared<MatSpD>(states, 1);
  eta->coeffRef(3, 0) = 1.0;
  MatSpDPtr mu1 = std::make_shared<MatSpD>(states, states);
  mu1->coeffRef(0, 1) = 1;
  mu1->coeffRef(0, 2) = 1;
  MatSpDPtr mu2 = std::make_shared<MatSpD>(states, states);
  mu2->coeffRef(1, 3) = 1;
  mu2->coeffRef(2, 3) = 1;
  std::vector<MatSpDPtr> mu = {mu1, mu2};
  return std::make_shared<WeightedAutomaton<MatSpD>>(states, characters, alpha,
                                                     mu, eta);
}

static inline auto gen_wa_hand_min_sparse()
-> std::shared_ptr<WeightedAutomaton<MatSpD>> {
  uint states = 3;
  uint characters = 2;
  MatSpDPtr alpha = std::make_shared<MatSpD>(1, states);
  alpha->coeffRef(0,0) = 1;
  MatSpDPtr eta = std::make_shared<MatSpD>(states, 1);
  eta->coeffRef(states - 1, 0) = 1;
  MatSpDPtr mu1 = std::make_shared<MatSpD>(states, states);
  mu1->coeffRef(0, 1) = 2;
  MatSpDPtr mu2 = std::make_shared<MatSpD>(states, states);
  mu2->coeffRef(1, 2) = 1;
  std::vector<MatSpDPtr > mu = {mu1, mu2};
  return std::make_shared<WeightedAutomaton<MatSpD>>(states, characters, alpha,
                                                     mu, eta);
}


#endif // STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONBENCHMARKS_H
