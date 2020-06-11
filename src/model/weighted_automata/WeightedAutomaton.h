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

#define EIGEN_USE_MKL_ALL 1
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/SPQRSupport>

#include "../../FloatingPointCompare.h"
#include "../RepresentationInterface.h"

const uint DEFAULT_RANDOM_RANGE_FACTOR = 300;

using MatSpDPtr = std::shared_ptr<Eigen::SparseMatrix<double, 0, long>>;
using MatSpIPtr = std::shared_ptr<Eigen::SparseMatrix<int, 0, long>>;
using MatSpI = Eigen::SparseMatrix<int, 0, long>;
using MatSpD = Eigen::SparseMatrix<double, 0, long>;


class WeightedAutomaton : public RepresentationInterface {
private:
  uint states{};
  uint noInputCharacters{};
  MatSpDPtr alpha;
  std::vector<MatSpDPtr> mu{};
  MatSpDPtr eta;

public:
  WeightedAutomaton();
  ~WeightedAutomaton() override;
  WeightedAutomaton(uint mStates, uint characters, MatSpDPtr mAlpha,
                    std::vector<MatSpDPtr> mMu,
                    MatSpDPtr mEta)
      : states(mStates), noInputCharacters(characters),
        alpha(std::move(mAlpha)), mu(std::move(mMu)), eta(std::move(mEta)) {}

  auto operator==(const std::shared_ptr<RepresentationInterface> &other) const
      -> bool override {
    auto mOther = std::static_pointer_cast<WeightedAutomaton>(other);
    return equivalent(*this, *mOther);
  }

  auto operator!=(const std::shared_ptr<RepresentationInterface> &other) const
      -> bool override {
    return !(*this == other);
  }

  [[nodiscard]] inline auto process_word(const std::vector<uint> &word) const
      -> double {
    MatSpD intermediate = *(this->alpha);
    for (const auto &letter : word) {
      if (letter >= this->noInputCharacters) {
        throw std::invalid_argument("The specified word has a letter that is "
                                    "not in the input alphabet!");
      }
      intermediate = (intermediate * *(this->mu[letter])).eval();
    }
    intermediate = (intermediate * *(this->eta)).eval();
    return intermediate.sum();
  }

  [[nodiscard]] inline auto get_states() const -> uint { return this->states; }

  [[nodiscard]] inline auto get_number_input_characters() const -> uint {
    return this->noInputCharacters;
  }

  [[nodiscard]] inline auto get_alpha() const -> const MatSpDPtr & {
    return this->alpha;
  }

  [[nodiscard]] inline auto get_mu() const
      -> const std::vector<MatSpDPtr> & {
    return this->mu;
  }

  [[nodiscard]] inline auto get_eta() const -> const MatSpDPtr & {
    return this->eta;
  }

  static auto create_subtraction_automaton(const WeightedAutomaton &lhs,
                                           const WeightedAutomaton &rhs)
      -> std::shared_ptr<WeightedAutomaton> {
    uint lhsStates = lhs.get_states();
    uint rhsStates = rhs.get_states();
    uint subStates = lhsStates + rhsStates;
    uint subCharacters = std::max(lhs.get_number_input_characters(),
                                  rhs.get_number_input_characters());

    MatSpDPtr lhsAlpha = lhs.get_alpha();
    MatSpDPtr rhsAlpha = rhs.get_alpha();
    MatSpDPtr subAlpha =
        std::make_shared<MatSpD>(1, lhsAlpha->cols() + rhsAlpha->cols());
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(subAlpha, lhsAlpha, rhsAlpha)
    for (long i = 0; i < lhsAlpha->cols() + rhsAlpha->cols(); i++) {
      if (i < lhsAlpha->cols()) {
        subAlpha->coeffRef(0, i) = lhsAlpha->coeffRef(0, i);
      } else {
        subAlpha->coeffRef(0, i) = -rhsAlpha->coeffRef(0, i - lhsAlpha->cols());
      }
    }

    MatSpDPtr lhsEta = lhs.get_eta();
    MatSpDPtr rhsEta = rhs.get_eta();
    MatSpDPtr subEta =
        std::make_shared<MatSpD>(lhsEta->rows() + rhsEta->rows(), 1);
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(subEta, lhsEta, rhsEta)
    for (long i = 0; i < lhsEta->rows() + rhsEta->rows(); i++) {
      if (i < lhsEta->rows()) {
        subEta->coeffRef(i, 0) = lhsEta->coeffRef(i, 0);
      } else {
        subEta->coeffRef(i, 0) = rhsEta->coeffRef(i - lhsEta->rows(), 0);
      }
    }

    std::vector<MatSpDPtr> subMu = {};
    std::mutex subMuMutex = std::mutex();
    std::vector<MatSpDPtr> lhsMu = lhs.get_mu();
    std::vector<MatSpDPtr> rhsMu = rhs.get_mu();
    MatSpDPtr muX;
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(subMu, subMuMutex, lhsMu, rhsMu, subStates, rhs, lhsStates, subCharacters, lhsStates, rhsStates) private(muX)
    for (size_t i = 0; i < subCharacters; i++) {
      muX = std::make_shared<MatSpD>(subStates, subStates);
      if (i < lhsMu.size()) {
        set_block(0, 0, lhsStates, lhsMu[i], muX);
      }
      if (i < rhsMu.size()) {
        set_block(lhsStates, lhsStates, rhsStates, rhsMu[i], muX);
      }
      std::lock_guard<std::mutex> guard(subMuMutex);
      subMu.push_back(muX);
    }
    return std::make_shared<WeightedAutomaton>(subStates, subCharacters,
                                                  subAlpha, subMu, subEta);
  }

  static inline void set_block(long startX, long startY, uint length,
                               const MatSpDPtr &source,
                               const MatSpDPtr &target) {
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
  generate_random_vectors(std::shared_ptr<WeightedAutomaton> &A, uint K,
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

  static auto equivalent(const WeightedAutomaton &lhs,
                         const WeightedAutomaton &rhs,
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
    MatSpD v = *(subtractionAutomaton->get_eta());
    std::vector<MatSpDPtr> sMu = subtractionAutomaton->get_mu();
    MatSpDPtr sAlpha = subtractionAutomaton->get_alpha();

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

WeightedAutomaton::WeightedAutomaton() {}

WeightedAutomaton::~WeightedAutomaton() {}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATON_H
