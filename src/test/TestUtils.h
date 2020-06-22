#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_TESTUTILS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_TESTUTILS_H

#include "../model/weighted_automata/WeightedAutomatonModel.h"

template <typename M, typename N>
static inline auto convert_N_M(const N &mat) -> std::shared_ptr<M> {
  std::shared_ptr<M> result = std::make_shared<M>(mat.rows(), mat.cols());
  for (long i = 0; i < mat.rows(); i++) {
    for (long j = 0; j < mat.cols(); j++) {
      result->coeffRef(i, j) = mat.coeff(i, j);
    }
  }
  return result;
}

static inline auto gen_wa_min_dense()
-> std::shared_ptr<WeightedAutomaton<MatDenD>> {
    uint states = 3;
    uint characters = 2;
    MatDenDPtr alpha = std::make_shared<MatDenD>(1, states);
    *alpha << 0,  800, 3600;
    MatDenDPtr eta = std::make_shared<MatDenD>(states, 1);
    *eta << 1, 0, 0;
    MatDenDPtr mu1 = std::make_shared<MatDenD>(states, states);
    *mu1 << -1.54231e-31,  -6.2105e-16, -8.33515e-16, -1.49588e-17, -0.06, -0.08, 1.12191e-17, 0.045, 0.06;
    MatDenDPtr mu2 = std::make_shared<MatDenD>(states, states);
    *mu2 << 1.63434e-17,  1.94083e-16,  2.59748e-16, 0.0249307, -3.27224e-16, -8.22317e-17, -0.00554017,  7.27164e-17,  1.82737e-17;
    std::vector<MatDenDPtr> mu = {mu1, mu2};
    return std::make_shared<WeightedAutomaton<MatDenD>>(states, characters, alpha,
                                                        mu, eta, 1);
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
                                                      mu, eta, 1);
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
  mu1->coeffRef(1, 3) = 1;
  mu1->coeffRef(2, 3) = 1;
  std::vector<MatSpDPtr> mu = {mu1, mu2};
  return std::make_shared<WeightedAutomaton<MatSpD>>(states, characters, alpha,
                                                     mu, eta, 0);
}

static inline auto gen_fixed_rand_v() -> std::vector<MatSpDPtr> {
  MatDenD mat1(2, 4);
  MatDenD mat2(2, 4);
  MatDenD mat3(2, 4);
  MatDenD mat4(2, 4);
  mat1 << 9, 5, 5, 7, 6, 11, 2, 1;
  mat2 << 2, 3, 1, 2, 12, 3, 9, 4;
  mat3 << 2, 7, 9, 10, 1, 11, 2, 6;
  mat4 << 4, 5, 2, 10, 5, 9, 5, 5;

  return {
      convert_N_M<MatSpD, MatDenD>(mat1), convert_N_M<MatSpD, MatDenD>(mat2),
      convert_N_M<MatSpD, MatDenD>(mat3), convert_N_M<MatSpD, MatDenD>(mat4)};
}

static inline auto gen_fixed_rand_v3() -> std::vector<MatSpDPtr> {
  MatDenD mat1(2, 3);
  MatDenD mat2(2, 3);
  MatDenD mat3(2, 3);
  mat1 << 1, 6, 2, 4, 5, 4;
  mat2 << 8, 1, 8, 7, 8, 2;
  mat3 << 1, 6, 5, 9, 2, 1;

  return {convert_N_M<MatSpD, MatDenD>(mat1),
          convert_N_M<MatSpD, MatDenD>(mat2),
          convert_N_M<MatSpD, MatDenD>(mat3)};
}
#endif // STOCHASTIC_SYSTEM_MINIMIZATION_TESTUTILS_H
