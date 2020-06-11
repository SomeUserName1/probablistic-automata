#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_TYPEDEFS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_TYPEDEFS_H

#include <eigen3/Eigen/src/SparseCore/SparseMatrix.h>
#include <iostream>
const uint DEFAULT_RANDOM_RANGE_FACTOR = 30;

using MatSpDPtr = std::shared_ptr<Eigen::SparseMatrix<double, 0, long>>;
using MatSpIPtr = std::shared_ptr<Eigen::SparseMatrix<int, 0, long>>;
using MatSpI = Eigen::SparseMatrix<int, 0, long>;
using MatSpD = Eigen::SparseMatrix<double, 0, long>;
using MatDenD = Eigen::MatrixXd;
using MatDenDPtr = std::shared_ptr<Eigen::MatrixXd>;
using MatDenI = Eigen::MatrixXi;
using MatDenIPtr = std::shared_ptr<Eigen::MatrixXi>;

template <typename T> concept Arithmetic = std::is_arithmetic<T>::value;

template <typename T>
concept Matrix = requires(T a, T b, long i, long j,
                          Eigen::SparseMatrix<double, 0, long> c) {
  a.coeffRef(i, j);
  a *b;
  a + b;
  a.rows();
  a.cols();
  std::cout << a;
  a * c;
  a + c;
  a.transpose();
  a.sum();
  a.col(0);
};


#endif // STOCHASTIC_SYSTEM_MINIMIZATION_TYPEDEFS_H
