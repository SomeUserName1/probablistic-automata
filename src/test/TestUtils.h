#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_TESTUTILS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_TESTUTILS_H

#include "../models/weighted_automata/WeightedAutomatonModel.h"

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

static inline auto
generate_words(unsigned int n, unsigned int chars,
               std::vector<std::vector<unsigned int>> &result) -> void {
  if (n == 1) {
    for (unsigned int j = 0; j < chars; j++) {
      result.push_back({j});
    }
  } else {
    generate_words(n - 1, chars, result);
    std::vector<std::vector<unsigned int>> iteratorCopy(result);

    for (const auto &word : iteratorCopy) {
      if (word.size() == n - 1) {
        for (unsigned int j = 0; j < chars; j++) {
          std::vector<unsigned int> temp(word);
          temp.push_back(j);
          result.push_back(temp);
        }
      }
    }
  }
}
#endif // STOCHASTIC_SYSTEM_MINIMIZATION_TESTUTILS_H
