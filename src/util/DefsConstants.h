#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_DEFSCONSTANTS_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_DEFSCONSTANTS_H

#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <memory>
const uint DEFAULT_RANDOM_RANGE_FACTOR = 10;
const uint PRINT_PRECISION = 8;
const std::array<unsigned long long int, 21> FACTORIALS = {
    1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362800, 3628800,
    39916800, 479001600,  6227020800, 87178291200, 1307674368000,
    20922789888000, 355687428096000, 6402373705728000, 121645100408832000,
    2432902008176640000
};

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


#endif // STOCHASTIC_SYSTEM_MINIMIZATION_DEFSCONSTANTS_H
