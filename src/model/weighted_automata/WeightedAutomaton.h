#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATON_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATON_H

#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <utility>
#include <variant>
#include <vector>

#define EIGEN_USE_MKL_ALL
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/SPQRSupport>

#include "../../FloatingPointCompare.h"
#include "../RepresentationInterface.h"

const uint DEFAULT_RANDOM_RANGE_FACTOR = 300;

using MatSpDPtr = std::shared_ptr<Eigen::SparseMatrix<double, 0, long>>;
using MatSpIPtr = std::shared_ptr<Eigen::SparseMatrix<int, 0, long>>;
using MatSpI = Eigen::SparseMatrix<int, 0, long>;
using MatSpD = Eigen::SparseMatrix<double, 0, long>;

/*template <typename T>
concept Matrix = requires(T a, T b, long i, long j,
                          Eigen::SparseMatrix<double, 0, long> c) {
  a.coeffRef(i, j);
  a *b;
  a + b;
  a.rows();
  a.cols();
  std::cout << a;
  a *c;
  a + c;
  a.transpose();
  a.sum();
  a.col(0);
};
*/

template <typename M> class WeightedAutomaton : public RepresentationInterface {
private:
  uint states{};
  uint noInputCharacters{};
  std::shared_ptr<M> alpha;
  std::vector<std::shared_ptr<M>> mu{};
  std::shared_ptr<M> eta;
  size_t dense;

public:
  WeightedAutomaton();
  ~WeightedAutomaton() override;
  WeightedAutomaton(uint mStates, uint characters, std::shared_ptr<M> mAlpha,
                    std::vector<std::shared_ptr<M>> mMu,
                    std::shared_ptr<M> mEta, size_t mDense)
      : states(mStates), noInputCharacters(characters),
        alpha(std::move(mAlpha)), mu(std::move(mMu)), eta(std::move(mEta)), dense(mDense) {}

  auto operator==(const std::shared_ptr<RepresentationInterface> &other) const
      -> bool override {
    auto mOther = std::static_pointer_cast<WeightedAutomaton<M>>(other);
    return equivalent(*this, *mOther);
  }

  auto operator!=(const std::shared_ptr<RepresentationInterface> &other) const
      -> bool override {
    return !(*this == other);
  }

  [[nodiscard]] inline auto process_word(const std::vector<uint> &word) const
      -> double {
    M intermediate = *(this->alpha);
    for (const auto &letter : word) {
      if (letter >= this->noInputCharacters) {
        throw std::invalid_argument("The specified word has a letter that is "
                                    "not in the input alphabet!");
      }
      intermediate = (intermediate * *(this->mu[letter])).eval();
    }
    return static_cast<double>((intermediate * *(this->eta)).eval());
  }

  [[nodiscard]] inline auto get_states() const -> uint { return this->states; }

  [[nodiscard]] inline auto get_number_input_characters() const -> uint {
    return this->noInputCharacters;
  }

  [[nodiscard]] inline auto get_alpha() const -> const std::shared_ptr<M> & {
    return this->alpha;
  }

  [[nodiscard]] inline auto get_mu() const
      -> const std::vector<std::shared_ptr<M>> & {
    return this->mu;
  }

  [[nodiscard]] inline auto get_eta() const -> const std::shared_ptr<M> & {
    return this->eta;
  }

  [[nodiscard]] inline auto is_dense() const -> bool {
    return this->dense;
  }

  static auto create_subtraction_automaton(const WeightedAutomaton<M> &lhs,
                                           const WeightedAutomaton<M> &rhs)
      -> std::shared_ptr<WeightedAutomaton<M>> {
    uint lhsStates = lhs.get_states();
    uint rhsStates = rhs.get_states();
    uint subStates = lhsStates + rhsStates;
    uint subCharacters = std::max(lhs.get_number_input_characters(),
                                  rhs.get_number_input_characters());

    std::shared_ptr<M> lhsAlpha = lhs.get_alpha();
    std::shared_ptr<M> rhsAlpha = rhs.get_alpha();
    std::shared_ptr<M> subAlpha =
        std::make_shared<M>(1, lhsAlpha->cols() + rhsAlpha->cols());
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(subAlpha, lhsAlpha, rhsAlpha)
    for (long i = 0; i < lhsAlpha->cols() + rhsAlpha->cols(); i++) {
      if (i < lhsAlpha->cols()) {
        subAlpha->coeffRef(0, i) = lhsAlpha->coeffRef(0, i);
      } else {
        subAlpha->coeffRef(0, i) = -rhsAlpha->coeffRef(0, i - lhsAlpha->cols());
      }
    }

    std::shared_ptr<M> lhsEta = lhs.get_eta();
    std::shared_ptr<M> rhsEta = rhs.get_eta();
    std::shared_ptr<M> subEta =
        std::make_shared<M>(lhsEta->rows() + rhsEta->rows(), 1);
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(subEta, lhsEta, rhsEta)
    for (long i = 0; i < lhsEta->rows() + rhsEta->rows(); i++) {
      if (i < lhsEta->rows()) {
        subEta->coeffRef(i, 0) = lhsEta->coeffRef(i, 0);
      } else {
        subEta->coeffRef(i, 0) = rhsEta->coeffRef(i - lhsEta->rows(), 0);
      }
    }

    std::vector<std::shared_ptr<M>> subMu = {};
    std::mutex subMuMutex = std::mutex();
    std::vector<std::shared_ptr<M>> lhsMu = lhs.get_mu();
    std::vector<std::shared_ptr<M>> rhsMu = rhs.get_mu();
    std::shared_ptr<M> muX;
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(subMu, subMuMutex, lhsMu, rhsMu, subStates, rhs, lhsStates, subCharacters, lhsStates, rhsStates) private(muX)
    for (size_t i = 0; i < subCharacters; i++) {
      muX = std::make_shared<M>(subStates, subStates);
      if (i < lhsMu.size()) {
        set_block(0, 0, lhsStates, lhsMu[i], muX);
      }
      if (i < rhsMu.size()) {
        set_block(lhsStates, lhsStates, rhsStates, rhsMu[i], muX);
      }
      std::lock_guard<std::mutex> guard(subMuMutex);
      subMu.push_back(muX);
    }
    return std::make_shared<WeightedAutomaton<M>>(subStates, subCharacters,
                                                  subAlpha, subMu, subEta, lhs.is_dense());
  }

  static inline void set_block(long startX, long startY, uint length,
                               const std::shared_ptr<M> &source,
                               const std::shared_ptr<M> &target) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(startX, startY, length, source, target)
    for (long i = 0; i < length; i++) {
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(startX, startY, length, source, target, i)
      for (long j = 0; j < length; j++) {
        target->coeffRef(startX + i, startY + j) = source->coeff(i, j);
      }
    }
  }

  [[nodiscard]] inline auto pretty_print() const -> std::string override {
    std::stringstream result;
    Eigen::IOFormat fmt(Eigen::StreamPrecision, 0, ", ", ";\n", "[", "]", "[",
                        "]");
    int i = 0;

    result << "No. States: " << this->states << " , No. Input Characters "
           << this->noInputCharacters << std::endl
           << "Initial Vector :" << std::endl
           << Eigen::MatrixXd(*(this->alpha)).format(fmt) << std::endl
           << "Transition Matrices: " << std::endl;
    for (const auto &mat : this->mu) {
      result << "Mu" << i << std::endl
             << Eigen::MatrixXd(*mat).format(fmt) << std::endl;
      i++;
    }
    result << "Final Vector: " << std::endl
           << Eigen::MatrixXd(*(this->eta)).format(fmt) << std::endl
           << std::endl;
    return result.str();
  }

  static inline auto
  generate_random_vectors(std::shared_ptr<WeightedAutomaton<M>> &A, uint K,
                          bool seeded = false, uint seed = 0)
      -> std::vector<MatSpIPtr> {
    std::mt19937 rng = std::mt19937(seed);
    if (!seeded) {
      std::random_device rd;
      rng = std::mt19937(rd());
    }
    std::uniform_int_distribution<> uniform(
        1, static_cast<int>(A->get_states() * A->get_states() * K));
    std::vector<MatSpIPtr> randV = {};
    MatSpIPtr vect;
    std::mutex randVMutex = std::mutex();

#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, uniform, randV, rng, randVMutex) private(vect)
    for (uint i = 0; i < A->get_states(); i++) {
      vect = std::make_shared<MatSpI>(1, A->get_number_input_characters());
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, uniform, randV, rng, vect, randVMutex)
      for (uint j = 0; j < A->get_number_input_characters(); j++) {
        vect->coeffRef(0, j) = uniform(rng);
      }
      std::lock_guard<std::mutex> guard(randVMutex);
      randV.push_back(vect);
    }
    return randV;
  }

  static auto equivalent(const WeightedAutomaton<M> &lhs,
                         const WeightedAutomaton<M> &rhs,
                         uint K = DEFAULT_RANDOM_RANGE_FACTOR) -> bool {
    if (lhs.get_number_input_characters() !=
        rhs.get_number_input_characters()) {
      return false;
    }
    auto subtractionAutomaton = create_subtraction_automaton(lhs, rhs);

    if (!floating_point_compare(((*(subtractionAutomaton->get_alpha()) *
                                  *(subtractionAutomaton->get_eta()))
                                     .eval()).sum(), 0.0)) {
      return false;
    }

    std::vector<MatSpIPtr> randomVectors =
        generate_random_vectors(subtractionAutomaton, K);
    M v = *(subtractionAutomaton->get_eta());
    std::vector<std::shared_ptr<M>> sMu = subtractionAutomaton->get_mu();
    std::shared_ptr<M> sAlpha = subtractionAutomaton->get_alpha();

    for (size_t i = 0; i < subtractionAutomaton->get_states(); i++) {
      for (uint j = 0; j < sMu.size(); j++) {
        v += ((randomVectors[i])->coeffRef(1, j) * (*(sMu[j]) * v).eval())
                 .eval();
      }
      if (!floating_point_compare(((*sAlpha * v).eval()).sum(), 0.0)) {
        return false;
      }
    }
    return true;
  }
};

template <typename M> WeightedAutomaton<M>::WeightedAutomaton() {}

template <typename M> WeightedAutomaton<M>::~WeightedAutomaton() {}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATON_H
