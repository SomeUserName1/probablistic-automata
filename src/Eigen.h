#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_EIGEN_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_EIGEN_H

#include <eigen3/Eigen/Eigen>

class MatrixSp : public Eigen::SparseMatrix<double, 0, long> {
    // FIXME meml and ub, maybe rc
public:
    inline MatrixSp(Eigen::Index rows, Eigen::Index cols) : Eigen::SparseMatrix<double, 0, long>(rows, cols) {}

    inline MatrixSp(Eigen::MatrixXd &other) : Eigen::SparseMatrix<double, 0, long>(other.rows(), other.cols()) {
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (long i = 0; i < other.rows(); i++) {
#pragma omp parallel for num_threads(THREADS) if(!TEST)
            for (long j = 0; j < other.cols(); j++) {
                this->coeffRef(i, j) = other(i, j);
            }
        }
    }

    inline bool isZero() {
        return this->nonZeros();
    }

    inline double &operator()(Eigen::Index row, Eigen::Index column) {
        return this->coeffRef(row, column);
    }
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_EIGEN_H
