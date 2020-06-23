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

// #define EIGEN_USE_MKL_ALL
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/OrderingMethods>
#include <eigen3/Eigen/SPQRSupport>

#include "../../ui/UserInterface.h"
#include "../../util/FloatingPointCompare.h"
#include "../../util/TypeDefs.h"
#include "../RepresentationInterface.h"

template <Matrix M> class WeightedAutomaton : public RepresentationInterface {
private:
  uint states{};
  uint noInputCharacters{};
  std::shared_ptr<M> alpha;
  std::vector<std::shared_ptr<M>> mu{};
  std::shared_ptr<M> eta;
  size_t dense;

public:
  WeightedAutomaton();
  WeightedAutomaton(const WeightedAutomaton &copy) = default;
  ~WeightedAutomaton() override;
  WeightedAutomaton(uint mStates, uint characters, std::shared_ptr<M> mAlpha,
                    std::vector<std::shared_ptr<M>> mMu,
                    std::shared_ptr<M> mEta, size_t mDense)
      : states(mStates), noInputCharacters(characters),
        alpha(std::move(mAlpha)), mu(std::move(mMu)), eta(std::move(mEta)),
        dense(mDense) {}

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
    return (intermediate * *(this->eta)).eval().sum();
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

  [[nodiscard]] inline auto is_dense() const -> bool { return this->dense; }

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
    shared(subMu, subMuMutex, lhsMu, rhsMu, subStates, rhs, lhsStates,         \
           subCharacters, lhsStates, rhsStates) private(muX)
    for (size_t i = 0; i < subCharacters; i++) {
      muX = std::make_shared<M>(subStates, subStates);
      muX->setZero();
      if (i < lhsMu.size()) {
        set_block(0, 0, lhsStates, lhsMu[i], muX);
      }
      if (i < rhsMu.size()) {
        set_block(lhsStates, lhsStates, rhsStates, rhsMu[i], muX);
      }
      std::lock_guard<std::mutex> guard(subMuMutex);
      subMu.push_back(muX);
    }
    return std::make_shared<WeightedAutomaton<M>>(
        subStates, subCharacters, subAlpha, subMu, subEta, lhs.is_dense());
  }

  static inline void set_block(long startX, long startY, uint length,
                               const std::shared_ptr<M> &source,
                               const std::shared_ptr<M> &target) {
    for (long i = 0; i < length; i++) {

      for (long j = 0; j < length; j++) {
        target->coeffRef(startX + i, startY + j) = source->coeff(i, j);
      }
    }
  }

  [[nodiscard]] inline auto pretty_print() const -> std::string override {
    std::stringstream result;
    size_t i = 0;
    Eigen::IOFormat fmt(Eigen::StreamPrecision, 0, ", ", ",\n\t", "(", ")", "(",
                        ")");
    result << "input=dense;\n"
           << "states=" << this->states
           << ";\ncharacters=" << this->noInputCharacters << ";\n"
           << "alpha=" << MatDenD(*(this->alpha)).format(fmt) << ";\n"
           << "mu=(" << std::endl;
    for (const auto &mat : this->mu) {
      result << "\t" << MatDenD(*mat).format(fmt);
      i++;
      if (i < (this->mu).size()) {
        result << ",\n\n";
      }
    }
    result << "\n);\n"
           << "eta=\n\t" << MatDenD(*(this->eta)).format(fmt) << ";\n"
           << std::endl;
    return result.str();
  }

  static inline auto
  generate_random_vectors(std::shared_ptr<WeightedAutomaton<M>> &A, uint K,
                          bool seeded = false, uint seed = 0)
      -> std::vector<MatSpDPtr> {
    auto rng = std::mt19937(seed);
    if (!seeded) {
      std::random_device rd;
      rng = std::mt19937(rd());
    }
    int max = static_cast<int>(A->get_states() * A->get_states() * K);
    std::uniform_int_distribution<> uniform(1, max);
    std::vector<MatSpDPtr> randV = {};
    MatSpDPtr vect;
    std::mutex randVMutex = std::mutex();

#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(A, uniform, randV, rng, randVMutex, max) private(vect)
    for (uint i = 0; i < A->get_states(); i++) {
      vect = std::make_shared<MatSpD>(1, A->get_number_input_characters());
      for (uint j = 0; j < A->get_number_input_characters(); j++) {
        vect->coeffRef(0, j) =
            (static_cast<double>(uniform(rng)) / static_cast<double>(max));
      }
      std::lock_guard<std::mutex> guard(randVMutex);
      randV.push_back(vect);
    }
    return randV;
  }

  [[nodiscard]] auto
  equivalent(const std::shared_ptr<RepresentationInterface> &other) const
      -> bool override {
    auto rhs = std::static_pointer_cast<WeightedAutomaton<M>>(other);
    return equivalent(*this, *rhs);
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
                                     .eval())
                                    .sum(),
                                0.0)) {
      return false;
    }

    std::vector<MatSpDPtr> randomVectors =
        generate_random_vectors(subtractionAutomaton, K);
    M v = *(subtractionAutomaton->get_eta());
    std::vector<double> sums;
    std::vector<double> balancers;
    std::vector<double> ys;
    std::vector<double> ts;

    for (int l = 0; l < v.rows(); l++) {
      sums.push_back(0.0);
      balancers.push_back(0.0);
      ys.push_back(0.0);
      ts.push_back(0.0);
    }

    std::vector<std::shared_ptr<M>> sMu = subtractionAutomaton->get_mu();
    std::shared_ptr<M> sAlpha = subtractionAutomaton->get_alpha();
    M temp;
    double result = 0.0;

    for (size_t i = 0; i < subtractionAutomaton->get_states(); i++) {
      for (uint j = 0; j < sMu.size(); j++) {
        temp =
            ((randomVectors[i])->coeff(0, j) * (*(sMu[j]) * v).eval()).eval();

        for (size_t k = 0; k < sums.size(); k++) {
          ys[k] = temp.coeffRef(static_cast<long>(k), 0) - balancers[k];
          ts[k] = sums[k] + ys[k];
          balancers[k] = (ts[k] - sums[k]) - ys[k];
          sums[k] = ts[k];
          v.coeffRef(static_cast<long>(k), 0) = sums[k];
        }
      }
      result = ((*sAlpha * v).eval()).sum();
      if (!floating_point_compare(result, 0.0)) {
        return false;
      }
    }
    return true;
  }
};

template <Matrix M> WeightedAutomaton<M>::WeightedAutomaton() {}

template <Matrix M> WeightedAutomaton<M>::~WeightedAutomaton() {}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATON_H
