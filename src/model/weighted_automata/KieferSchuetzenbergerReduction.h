#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H

#include <iostream>
#include <memory>
#include <random>
#include <string>

#include "../../FloatingPointCompare.h"
#include "../ReductionMethodInterface.h"
#include "WeightedAutomaton.h"

template <Matrix M>
class KieferSchuetzenbergerReduction : public ReductionMethodInterface {
public:
  KieferSchuetzenbergerReduction();

  KieferSchuetzenbergerReduction(
      KieferSchuetzenbergerReduction &&move) noexcept = default;

  ~KieferSchuetzenbergerReduction() override;

  [[nodiscard]] inline auto get_name() const -> std::string override {
    return "Random Basis Schützenberger Reduction";
  }

  [[nodiscard]] inline auto
  reduce(const std::shared_ptr<RepresentationInterface> &waInstance) const
      -> std::shared_ptr<RepresentationInterface> override {
    return reduce(waInstance, DEFAULT_RANDOM_RANGE_FACTOR, false);
  }

  static auto reduce(const std::shared_ptr<RepresentationInterface> &waInstance,
                     uint K, bool seed = false)
      -> std::shared_ptr<RepresentationInterface> {
    auto WA = std::static_pointer_cast<WeightedAutomaton<M>>(waInstance);
    std::vector<MatSpDPtr> randomVectors = generate_random_vectors(WA, K, seed);
    std::shared_ptr<WeightedAutomaton<M>> minWA =
        forward_reduction(WA, randomVectors);
    randomVectors = generate_random_vectors(minWA, K, seed);
    minWA = backward_reduction(minWA, randomVectors);
    return std::move(minWA);
  }

  static auto
  backward_reduction(const std::shared_ptr<WeightedAutomaton<M>> &WA,
                     const std::vector<MatSpDPtr> &randomVectors)
      -> std::shared_ptr<WeightedAutomaton<M>> {
    std::vector<MatSpDPtr> rhoVectors =
        calculate_rho_backward_vectors(WA, randomVectors);
    MatSpD backwardBasis(WA->get_states(),
                         1 + static_cast<long>(rhoVectors.size()));

    fill_col(0, 0, WA->get_eta(), &backwardBasis);

    for (size_t i = 0; i < rhoVectors.size(); i++) {
      fill_col(0, static_cast<long>(i + 1), rhoVectors[i], &backwardBasis);
    }
    Eigen::SPQR<MatSpD> qr;
    qr.compute(backwardBasis);
    long rank = qr.rank();

    backwardBasis.conservativeResize(backwardBasis.rows(), rank);
    backwardBasis.makeCompressed();

    std::shared_ptr<M> alphaArrow =
        std::make_shared<M>((*(WA->get_alpha()) * backwardBasis).eval());
    std::shared_ptr<M> etaArrow = std::make_shared<M>(rank, 1);
    etaArrow->setZero();
    etaArrow->coeffRef(0, 0) = 1;

    std::vector<std::shared_ptr<M>> muArrow = {};
    std::mutex muArrowMutex = std::mutex();
    MatDenD muXArrow;
    MatDenD b;
    Eigen::SparseQR<MatSpD, Eigen::COLAMDOrdering<long>> qrX;
    qrX.compute(backwardBasis);

#pragma omp parallel for default(none)                                         \
    num_threads(THREADS) if (!TEST) private(muXArrow, b)                       \
        shared(qrX, muArrow, backwardBasis, WA, muArrowMutex)
    for (size_t i = 0; i < WA->get_mu().size(); i++) {
      b = (*(WA->get_mu()[i]) * backwardBasis).eval();
      muXArrow = (qrX.solve(b)).eval();

      std::lock_guard<std::mutex> guard(muArrowMutex);
      muArrow.push_back(convert_dense_M(muXArrow));
    }
    return std::make_shared<WeightedAutomaton<M>>(
        static_cast<uint>(rank), WA->get_number_input_characters(), alphaArrow,
        muArrow, etaArrow, WA->is_dense());
  }

  static auto forward_reduction(const std::shared_ptr<WeightedAutomaton<M>> &WA,
                                const std::vector<MatSpDPtr> &randomVectors)
      -> std::shared_ptr<WeightedAutomaton<M>> {
    std::vector<MatSpDPtr> rhoVectors =
        calculate_rho_forward_vectors(WA, randomVectors);
    MatSpD forwardBasis(1 + static_cast<long>(rhoVectors.size()),
                        WA->get_states());

    fill_row(0, 0, WA->get_alpha(), &forwardBasis);
    for (size_t i = 0; i < rhoVectors.size(); i++) {
      fill_row(0, static_cast<long>(i + 1), rhoVectors[i], &forwardBasis);
    }
    Eigen::SPQR<MatSpD> qr;
    qr.compute(forwardBasis);
    long rank = qr.rank();

    forwardBasis.conservativeResize(rank, forwardBasis.cols());
    forwardBasis.makeCompressed();

    std::shared_ptr<M> etaArrow =
        std::make_shared<M>((forwardBasis * *(WA->get_eta())).eval());
    std::shared_ptr<M> alphaArrow = std::make_shared<M>(1, rank);
    alphaArrow->setZero();
    alphaArrow->coeffRef(0, 0) = 1;

    std::vector<std::shared_ptr<M>> muArrow = {};
    std::mutex muArrowMutex = std::mutex();
    Eigen::SparseQR<MatSpD, Eigen::COLAMDOrdering<long>> qrX;
    MatDenD b;
    MatDenD muXArrow;
    MatSpD A = MatSpD(forwardBasis.transpose());
    A.makeCompressed();
    qrX.compute(A);

#pragma omp parallel for default(none)                                         \
    num_threads(THREADS) if (!TEST) private(b, muXArrow)                       \
        shared(qrX, muArrow, forwardBasis, WA, muArrowMutex)
    for (size_t i = 0; i < WA->get_mu().size(); i++) {
      // x*A = b <=> A.transpose() * z = b.transpose(); x = z.transpose()
      // => x = (housholder(A.transpose()).solve(b.transpose())).transpose()
      b = (((forwardBasis * *(WA->get_mu()[i])).eval()).transpose()).eval();
      muXArrow = (((qrX.solve(b)).eval()).transpose()).eval();

      std::lock_guard<std::mutex> guard(muArrowMutex);
      muArrow.push_back(convert_dense_M(muXArrow));
    }
    return std::make_shared<WeightedAutomaton<M>>(
        static_cast<uint>(rank), WA->get_number_input_characters(), alphaArrow,
        muArrow, etaArrow, WA->is_dense());
  }

  static auto calculate_rho_backward_vectors(
      const std::shared_ptr<WeightedAutomaton<M>> &WA,
      const std::vector<MatSpDPtr>
          &randomVectors) -> std::vector<MatSpDPtr> {
    std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> sigmaK =
        generate_words_backwards(WA, WA->get_states());
    std::vector<MatSpDPtr> result = {};
    std::mutex resultMutex = std::mutex();
    MatSpD vI;

#pragma omp parallel for default(none)                                         \
    num_threads(THREADS) if (!TEST) private(vI)                                \
        shared(result, sigmaK, randomVectors, resultMutex, WA)
    for (size_t j = 0; j < randomVectors.size(); j++) {
      vI = MatSpD(WA->get_states(), 1);
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(result, vI, sigmaK, randomVectors, resultMutex, j)
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

  static auto calculate_rho_forward_vectors(
      const std::shared_ptr<WeightedAutomaton<M>> &WA,
      const std::vector<MatSpDPtr>
          &randomVectors)
      -> std::vector<std::shared_ptr<Eigen::SparseMatrix<double, 0, long>>> {
    std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> sigmaK =
        generate_words_forwards(WA, WA->get_states());
    std::vector<MatSpDPtr> result = {};
    std::mutex resultMutex = std::mutex();
    MatSpD vI;

#pragma omp parallel for default(none)                                         \
    num_threads(THREADS) if (!TEST) private(vI)                                \
        shared(result, sigmaK, randomVectors, WA, resultMutex)
    for (size_t j = 0; j < randomVectors.size(); j++) {
      vI = MatSpD(1, WA->get_states());
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(result, vI, sigmaK, randomVectors, j, WA, resultMutex)
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
  generate_words_forwards(const std::shared_ptr<WeightedAutomaton<M>> &WA,
                          uint k)
      -> std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> {
    std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> result;
    std::mutex resultMutex = std::mutex();
    MatSpDPtr resultVect;

    if (k == 1) {
      result = {};
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(WA, resultMutex, result) private(resultVect)
      for (size_t i = 0; i < WA->get_mu().size(); i++) {
        resultVect = convert_dense_sparse(
            (*(WA->get_alpha()) * *(WA->get_mu()[i])).eval());

        if (!floating_point_compare(resultVect->sum(), 0.0)) {
          std::lock_guard<std::mutex> guard(resultMutex);
          result.emplace_back(resultVect, std::vector({static_cast<uint>(i)}));
        }
      }
    } else {
      result = generate_words_forwards(WA, k - 1);
      std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> iteratorCopy(
          result);

#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(WA, resultMutex, result, iteratorCopy, k) private(resultVect)
      for (size_t j = 0; j < iteratorCopy.size(); j++) {
        if (std::get<1>(iteratorCopy[j]).size() == k - 1) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(WA, resultMutex, result, j, iteratorCopy) private(resultVect)
          for (size_t i = 0; i < WA->get_mu().size(); i++) {
            resultVect = convert_dense_sparse(
                (*(std::get<0>(iteratorCopy[j])) * *(WA->get_mu()[i])).eval());
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
  generate_words_backwards(const std::shared_ptr<WeightedAutomaton<M>> &WA,
                           uint k)
      -> std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> {
    std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> result;
    std::mutex resultMutex = std::mutex();
    MatSpDPtr resultVect;

    if (k == 1) {
      result = {};
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(WA, resultMutex, result) private(resultVect)
      for (size_t i = 0; i < WA->get_mu().size(); i++) {
        resultVect = convert_dense_sparse(
            (*(WA->get_mu()[i]) * *(WA->get_eta())).eval());
        if (!floating_point_compare(resultVect->sum(), 0.0)) {
          std::lock_guard<std::mutex> guard(resultMutex);
          result.emplace_back(resultVect, std::vector({static_cast<uint>(i)}));
        }
      }
    } else {
      result = generate_words_backwards(WA, k - 1);
      std::vector<std::tuple<MatSpDPtr, std::vector<uint>>> iteratorCopy(
          result);
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(WA, resultMutex, result, iteratorCopy, k) private(resultVect)
      for (size_t j = 0; j < iteratorCopy.size(); j++) {
        if (std::get<1>(iteratorCopy[j]).size() == k - 1) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(WA, resultMutex, result, iteratorCopy, j) private(resultVect)
          for (size_t i = 0; i < WA->get_mu().size(); i++) {
            resultVect = convert_dense_sparse(
                (*(WA->get_mu()[i]) * *(std::get<0>(iteratorCopy[j]))).eval());
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
    for (long i = 0; i < mat.rows(); i++) {
      for (long j = 0; j < mat.cols(); j++) {
        result->coeffRef(i, j) = mat.coeff(i, j);
      }
    }
    result->makeCompressed();
    return result;
  }

  static inline auto convert_dense_M(const MatDenD &mat) -> std::shared_ptr<M> {
    std::shared_ptr<M> result = std::make_shared<M>(mat.rows(), mat.cols());
    for (long i = 0; i < mat.rows(); i++) {
      for (long j = 0; j < mat.cols(); j++) {
        result->coeffRef(i, j) = mat.coeff(i, j);
      }
    }
    return result;
  }

  static inline auto get_word_factor(
      const std::vector<uint> &word,
      const MatSpDPtr &randVector)
      -> double {
    double result = 1.0;
    for (uint i = 0; i < word.size(); i++) {
      result = result * randVector->coeffRef(word[i], i);
    }
    return result;
  }

  static inline auto
  generate_random_vectors(const std::shared_ptr<WeightedAutomaton<M>> &WA,
                          uint K, bool seeded = false, uint seed = 0)
      -> std::vector<MatSpDPtr> {
    auto rng = std::mt19937(seed);
    if (!seeded) {
      std::random_device rd;
      rng = std::mt19937(rd());
    }
    int max = static_cast<int>(WA->get_states() * WA->get_states() * K);
    std::uniform_int_distribution<> uniform(1, max);
    std::vector<MatSpDPtr> randV = {};
    MatSpDPtr vect;
    std::mutex randVMutex = std::mutex();

#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(WA, uniform, randV, rng, randVMutex, max) private(vect)
    for (uint i = 0; i < WA->get_states(); i++) {
      vect = std::make_shared<MatSpD>(WA->get_number_input_characters(), WA->get_states());
      for (uint j = 0; j < WA->get_number_input_characters(); j++) {
        for (uint k = 0; k < WA->get_states(); k++) {
          vect->coeffRef(j, k) =
              (static_cast<double>(uniform(rng)) / static_cast<double>(max));
        }
      }
      std::lock_guard<std::mutex> guard(randVMutex);
      randV.push_back(vect);
    }
    return randV;
  }

  template <typename T>
  static inline void fill_row(long rowSource, long rowTarget,
                              const std::shared_ptr<T> &source,
                              MatSpD *target) {
    for (long k = 0; k < source->cols(); k++) {
      target->coeffRef(rowTarget, k) = source->coeff(rowSource, k);
    }
  }

  template <typename T>
  static inline void fill_col(long colSource, long colTarget,
                              const std::shared_ptr<T> &source,
                              MatSpD *target) {
    for (long k = 0; k < source->rows(); k++) {
      target->coeffRef(k, colTarget) = source->coeff(k, colSource);
    }
  }
};

template <Matrix M>
KieferSchuetzenbergerReduction<M>::KieferSchuetzenbergerReduction() = default;

template <Matrix M>
KieferSchuetzenbergerReduction<M>::~KieferSchuetzenbergerReduction() = default;

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
