#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H

#include <iostream>
#include <memory>
#include <random>
#include <string>

#include "../../FloatingPointCompare.h"
#include "../ReductionMethodInterface.h"
#include "WeightedAutomaton.h"

template <typename M>
class KieferSchuetzenbergerReduction : public ReductionMethodInterface {
public:
  KieferSchuetzenbergerReduction();

  ~KieferSchuetzenbergerReduction() override;

  [[nodiscard]] inline auto get_name() const -> std::string override {
    return "Random Basis Schützenberger Reduction";
  }

  static inline auto get_word_factor(
      const std::vector<uint> &word,
      const std::shared_ptr<Eigen::SparseMatrix<int, 0, long>> &randVector)
      -> int {
    int result = 1;
    for (uint i = 0; i < word.size(); i++) {
      result = result * randVector->coeffRef(word[i], i);
    }
    return result;
  }

  static inline auto
  generate_random_vectors(const std::shared_ptr<WeightedAutomaton<M>> &A,
                          uint K, bool seeded = false, uint seed = 0)
      -> std::vector<MatSpIPtr> {
    auto rng = std::mt19937(seed);
    if (!seeded) {
      std::random_device rd;
      rng = std::mt19937(rd());
    }
    std::uniform_int_distribution<> uniform(
        1, static_cast<int>(A->get_states() * A->get_states() * K));
    std::vector<MatSpIPtr> randV = {};
    MatSpI vect;
    std::mutex randVMutex = std::mutex();

#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, uniform, randV, rng, randVMutex) private(vect)
    for (uint i = 0; i < A->get_states(); i++) {
      vect = MatSpI(A->get_number_input_characters(), A->get_states());
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, uniform, randV, vect, rng, randVMutex)
      for (uint j = 0; j < A->get_number_input_characters(); j++) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, uniform, randV, j, vect, rng, randVMutex)
        for (uint k = 0; k < A->get_states(); k++) {
          vect.coeffRef(j, k) = uniform(rng);
        }
      }
      std::lock_guard<std::mutex> guard(randVMutex);
      randV.push_back(std::make_shared<MatSpI>(vect));
    }
    return randV;
  }

  template <typename T>
  static inline void fill_row(long rowSource, long rowTarget,
                              const std::shared_ptr<T> &source,
                              MatSpD *target) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(rowSource, rowTarget, source, target)
    for (long k = 0; k < source->cols(); k++) {
      target->coeffRef(rowTarget, k) = source->coeff(rowSource, k);
    }
  }

  template <typename T>
  static inline void fill_col(long colSource, long colTarget,
                              const std::shared_ptr<T> &source,
                              MatSpD *target) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(source, target, colSource, colTarget)
    for (long k = 0; k < source->rows(); k++) {
      target->coeffRef(k, colTarget) = source->coeff(k, colSource);
    }
  }

  [[nodiscard]] inline auto
  reduce(const std::shared_ptr<RepresentationInterface> &waInstance) const
      -> std::shared_ptr<RepresentationInterface> override {
    return reduce(waInstance, DEFAULT_RANDOM_RANGE_FACTOR);
  }

  static auto reduce(const std::shared_ptr<RepresentationInterface> &waInstance,
                     uint K) -> std::shared_ptr<RepresentationInterface> {
    auto A = std::static_pointer_cast<WeightedAutomaton<M>>(waInstance);
    std::vector<MatSpIPtr> randomVectors = generate_random_vectors(A, K);
    std::shared_ptr<WeightedAutomaton<M>> minA =
        forward_reduction(A, randomVectors);
    randomVectors = generate_random_vectors(minA, K);
    minA = backward_reduction(minA, randomVectors);
    return std::move(minA);
  }

  static auto backward_reduction(const std::shared_ptr<WeightedAutomaton<M>> &A,
                                 const std::vector<MatSpIPtr> &randomVectors)
      -> std::shared_ptr<WeightedAutomaton<M>> {
    std::vector<MatSpDPtr> rhoVectors =
        calculate_rho_backward_vectors(A, randomVectors);
    MatSpD backwardBasis(A->get_states(),
                         1 + static_cast<long>(rhoVectors.size()));

    fill_col(0, 0, A->get_eta(), &backwardBasis);
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(backwardBasis, rhoVectors)
    for (size_t i = 0; i < rhoVectors.size(); i++) {
      fill_col(static_cast<long>(i + 1), 0, rhoVectors[i], &backwardBasis);
    }
    Eigen::SPQR<MatSpD> qr;
    qr.compute(backwardBasis);
    long rank = qr.rank();

    backwardBasis.conservativeResize(backwardBasis.rows(), rank);
    backwardBasis.makeCompressed();

    std::shared_ptr<M> alphaArrow =
        std::make_shared<M>((*(A->get_alpha()) * backwardBasis).eval());
    std::shared_ptr<M> etaArrow = std::make_shared<M>(rank, 1);
    etaArrow->coeffRef(0, 0) = 1;

    std::vector<std::shared_ptr<M>> muArrow = {};
    std::mutex muArrowMutex = std::mutex();
    Eigen::MatrixXd muXArrow;
    Eigen::MatrixXd b;
    Eigen::SPQR<MatSpD> qrX;
    qrX.compute(backwardBasis);

#pragma omp parallel for default(none)                                         \
    num_threads(THREADS) if (!TEST) private(qrX, muXArrow, b)                  \
        shared(muArrow, backwardBasis, A, muArrowMutex)
    for (size_t i = 0; i < A->get_mu().size(); i++) {
      b = (*(A->get_mu()[i]) * backwardBasis).eval();
      muXArrow = (qrX.solve(b)).eval();

      std::lock_guard<std::mutex> guard(muArrowMutex);
      muArrow.push_back(convert_dense_M(muXArrow));
    }
    return std::make_shared<WeightedAutomaton<M>>(
        static_cast<uint>(rank), A->get_number_input_characters(), alphaArrow,
        muArrow, etaArrow, A->is_dense());
  }

  static auto forward_reduction(const std::shared_ptr<WeightedAutomaton<M>> &A,
                                const std::vector<MatSpIPtr> &randomVectors)
      -> std::shared_ptr<WeightedAutomaton<M>> {
    std::vector<MatSpDPtr> rhoVectors =
        calculate_rho_forward_vectors(A, randomVectors);
    MatSpD forwardBasis(1 + static_cast<long>(rhoVectors.size()),
                        A->get_states());

    fill_row(0, 0, A->get_alpha(), &forwardBasis);
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(forwardBasis, rhoVectors)
    for (size_t i = 0; i < rhoVectors.size(); i++) {
      fill_row(static_cast<long>(i + 1), 0, rhoVectors[i], &forwardBasis);
    }
    Eigen::SPQR<MatSpD> qr;
    qr.compute(forwardBasis);
    long rank = qr.rank();

    forwardBasis.conservativeResize(rank, forwardBasis.cols());
    forwardBasis.makeCompressed();
    forwardBasis = MatSpD(forwardBasis.transpose());
    forwardBasis.makeCompressed();

    std::shared_ptr<M> etaArrow =
        std::make_shared<M>((forwardBasis * *(A->get_eta())).eval());
    std::shared_ptr<M> alphaArrow = std::make_shared<M>(1, rank);
    alphaArrow->coeffRef(0, 0) = 1;

    std::vector<std::shared_ptr<M>> muArrow = {};
    std::mutex muArrowMutex = std::mutex();
    Eigen::SPQR<MatSpD> qrX;
    Eigen::MatrixXd b;
    Eigen::MatrixXd muXArrow;
    qrX.compute(forwardBasis);

#pragma omp parallel for default(none)                                         \
    num_threads(THREADS) if (!TEST) private(qrX, b, muXArrow)                  \
        shared(muArrow, forwardBasis, A, muArrowMutex)
    for (size_t i = 0; i < A->get_mu().size(); i++) {
      // x*A = b <=> A.transpose() * z = b.transpose(); x = z.transpose()
      // => x = (housholder(A.transpose()).solve(b.transpose())).transpose()
      b = (((forwardBasis * *(A->get_mu()[i])).eval()).transpose()).eval();
      muXArrow = (((qrX.solve(b)).eval()).transpose()).eval();

      std::lock_guard<std::mutex> guard(muArrowMutex);
      muArrow.push_back(convert_dense_M(muXArrow));
    }
    return std::make_shared<WeightedAutomaton<M>>(
        static_cast<uint>(rank), A->get_number_input_characters(), alphaArrow,
        muArrow, etaArrow, A->is_dense());
  }

  static auto calculate_rho_backward_vectors(
      const std::shared_ptr<WeightedAutomaton<M>> &A,
      const std::vector<std::shared_ptr<Eigen::SparseMatrix<int, 0, long>>>
          &randomVectors) -> std::vector<MatSpDPtr> {
    std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> sigmaK =
        generate_words_backwards(A, A->get_states());
    std::vector<MatSpDPtr> result = {};
    std::mutex resultMutex = std::mutex();
    MatSpD vI = MatSpD(A->get_states(), 1);
#pragma omp parallel for default(none)                                         \
    num_threads(THREADS) if (!TEST) private(vI)                                \
        shared(result, sigmaK, randomVectors, resultMutex)
    for (size_t j = 0; j < randomVectors.size(); j++) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(result, vI, sigmaK, randomVectors, resultMutex, j)
      for (size_t i = 0; i < sigmaK.size(); i++) {
        vI += (*std::get<0>(sigmaK[i]) *
               get_word_factor(std::get<1>(sigmaK[i]), randomVectors[j]))
                  .eval();
      }
      std::lock_guard<std::mutex> guard(resultMutex);
      result.push_back(std::make_shared<MatSpD>(vI.transpose()));
    }
    return result;
  }

  static auto calculate_rho_forward_vectors(
      const std::shared_ptr<WeightedAutomaton<M>> &A,
      const std::vector<std::shared_ptr<Eigen::SparseMatrix<int, 0, long>>>
          &randomVectors)
      -> std::vector<std::shared_ptr<Eigen::SparseMatrix<double, 0, long>>> {
    std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> sigmaK =
        generate_words_forwards(A, A->get_states());
    std::vector<MatSpDPtr> result = {};
    std::mutex resultMutex = std::mutex();
    MatSpD vI;

#pragma omp parallel for default(none)                                         \
    num_threads(THREADS) if (!TEST) private(vI)                                \
        shared(result, sigmaK, randomVectors, A, resultMutex)
    for (size_t j = 0; j < randomVectors.size(); j++) {
      vI = MatSpD(1, A->get_states());
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(result, vI, sigmaK, randomVectors, j, A, resultMutex)
      for (size_t i = 0; i < sigmaK.size(); i++) {
        vI += (*std::get<0>(sigmaK[i]) *
               get_word_factor(std::get<1>(sigmaK[i]), randomVectors[j]))
                  .eval();
      }
      std::lock_guard<std::mutex> guard(resultMutex);
      result.push_back(std::make_shared<MatSpD>(vI));
    }
    return result;
  }

  static auto
  generate_words_forwards(const std::shared_ptr<WeightedAutomaton<M>> &A,
                          uint k)
      -> std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> {
    std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> result;
    std::mutex resultMutex = std::mutex();
    MatSpDPtr resultVect;

    if (k == 1) {
      result = {};
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, resultMutex, result) private(resultVect)
      for (size_t i = 0; i < A->get_mu().size(); i++) {
        resultVect = convert_dense_sparse(
            (*(A->get_alpha()) * *(A->get_mu()[i])).eval());

        if (!floating_point_compare(resultVect->sum(), 0.0)) {
          std::lock_guard<std::mutex> guard(resultMutex);
          result.emplace_back(resultVect, std::vector({static_cast<uint>(i)}));
        }
      }
    } else {
      result = generate_words_forwards(A, k - 1);
      std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> iteratorCopy(
          result);

#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, resultMutex, result, iteratorCopy, k) private(resultVect)
      for (size_t j = 0; j < iteratorCopy.size(); j++) {
        if (std::get<1>(iteratorCopy[j]).size() == k - 1) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, resultMutex, result, j, iteratorCopy) private(resultVect)
          for (size_t i = 0; i < A->get_mu().size(); i++) {
            resultVect = std::make_shared<MatSpD>(1, A->get_states());
            resultVect = convert_dense_sparse(
                (*(std::get<0>(iteratorCopy[j])) * *(A->get_mu()[i])).eval());
            if (!floating_point_compare(resultVect->sum(), 0.0)) {
              std::vector<uint> temp = std::get<1>(iteratorCopy[j]);
              temp.push_back(static_cast<uint>(i));
              std::lock_guard<std::mutex> guard(resultMutex);
              result.emplace_back(resultVect, temp);
            }
          }
        }
      }
    }
    return result;
  }

  static auto
  generate_words_backwards(const std::shared_ptr<WeightedAutomaton<M>> &A,
                           uint k)
      -> std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> {
    std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> result;
    std::mutex resultMutex = std::mutex();
    MatSpDPtr resultVect;

    if (k == 1) {
      result = {};
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, resultMutex, result) private(resultVect)
      for (size_t i = 0; i < A->get_mu().size(); i++) {
        resultVect = convert_dense_sparse(
            (*(A->get_mu()[i]) * *(A->get_eta())).eval());
        if (!floating_point_compare(resultVect->sum(), 0.0)) {
          std::lock_guard<std::mutex> guard(resultMutex);
          result.emplace_back(resultVect, std::vector({static_cast<uint>(i)}));
        }
      }
    } else {
      result = generate_words_backwards(A, k - 1);
      std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> iteratorCopy(
          result);
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, resultMutex, result, iteratorCopy, k) private(resultVect)
      for (size_t j = 0; j < iteratorCopy.size(); j++) {
        if (std::get<1>(iteratorCopy[j]).size() == k - 1) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, resultMutex, result, iteratorCopy, j) private(resultVect)
          for (size_t i = 0; i < A->get_mu().size(); i++) {
            resultVect = convert_dense_sparse(
                (*(A->get_mu()[i]) * *(std::get<0>(iteratorCopy[j]))).eval());
            if (!floating_point_compare(resultVect->sum(), 0.0)) {
              auto temp = std::get<1>(iteratorCopy[j]);
              temp.insert(temp.begin(), static_cast<uint>(i));
              std::lock_guard<std::mutex> guard(resultMutex);
              result.emplace_back(resultVect, temp);
            }
          }
        }
      }
    }
    return result;
  }

  static inline auto convert_dense_sparse(const M &mat) -> MatSpDPtr {
    std::shared_ptr<MatSpD> result =
        std::make_shared<MatSpD>(mat.rows(), mat.cols());
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(result, mat)
    for (long i = 0; i < mat.rows(); i++) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(result, mat, i)
      for (long j = 0; j < mat.cols(); j++) {
        result->coeffRef(i, j) = mat.coeff(i, j);
      }
    }
    result->makeCompressed();
    return result;
  }

  static inline auto convert_dense_M(const Eigen::MatrixXd &mat)
      -> std::shared_ptr<M> {
    std::shared_ptr<M> result = std::make_shared<M>(mat.rows(), mat.cols());
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(result, mat)
    for (long i = 0; i < mat.rows(); i++) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(result, mat, i)
      for (long j = 0; j < mat.cols(); j++) {
        result->coeffRef(i, j) = mat.coeff(i, j);
      }
    }
    return result;
  }
};

template <typename M>
KieferSchuetzenbergerReduction<M>::KieferSchuetzenbergerReduction() {}

template <typename M>
KieferSchuetzenbergerReduction<M>::~KieferSchuetzenbergerReduction() {}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
