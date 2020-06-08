#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H

#include <utility>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <random>
#include <mutex>
#include <variant>

#include "../../Eigen.h"
#include "../RepresentationInterface.h"

template <typename T>
concept Matrix = requires (T a, T b, long i, long j, Eigen::MatrixXd c) {
    a(i, j); a * b; a + b; a.rows(); a.cols(); std::cout << a; a * c; a + c; a.transpose(); a.isZero(); };

template <Matrix M>
class WeightedAutomatonInstance : public RepresentationInterface {
private:
    uint states{};
    uint noInputCharacters{};
    std::shared_ptr<M> alpha;
    std::vector<std::shared_ptr<M>> mu;
    std::shared_ptr<M> eta;

public:
    WeightedAutomatonInstance();
    ~WeightedAutomatonInstance() override;
    WeightedAutomatonInstance(uint mStates, uint characters, std::shared_ptr<M> mAlpha,
            std::vector<std::shared_ptr<M>> mMu, std::shared_ptr<M> mEta)
        : states(mStates), noInputCharacters(characters), alpha(std::move(mAlpha)), mu(std::move(mMu)),
                                 eta(std::move(mEta)) {}

    bool operator==(const RepresentationInterface &other) const override {
        auto mOther = static_cast<WeightedAutomatonInstance<M>>(other);
        return equivalent(*this, mOther);
    }

    bool operator!=(const RepresentationInterface &other) const override {
        return !(*this == other);
    }

    double process_word(const std::vector<uint> &word) const {
        auto intermediate = Eigen::MatrixXd(*(this->alpha));
        for (const auto &letter : word) {
            if (letter >= this->noInputCharacters) {
                throw std::invalid_argument("The specified word has a letter that is not in the input alphabet!");
            }
            intermediate = intermediate * *(this->mu[letter]);
        }
        return static_cast<double>(intermediate * *(this->eta));
    }

    uint get_states() const {
        return this->states;
    }

    uint get_number_input_characters() const {
        return this->noInputCharacters;
    }

    const std::shared_ptr<M> &get_alpha() const {
        return this->alpha;
    }

    const std::vector<std::shared_ptr<M>> &get_mu() const {
        return this->mu;
    }

    const std::shared_ptr<M> &get_eta() const {
        return this->eta;
    }

    static const std::shared_ptr<WeightedAutomatonInstance<M>> create_subtraction_automaton(
            const WeightedAutomatonInstance<M> &lhs, const WeightedAutomatonInstance<M> &rhs) {
        // TODO if not the same format convert
        uint lhsStates = lhs.get_states();
        uint rhsStates = rhs.get_states();
        uint subStates = lhsStates + rhsStates;
        uint subCharacters = std::max(lhs.get_number_input_characters(), rhs.get_number_input_characters());

        auto lhsAlpha = lhs.get_alpha();
        auto rhsAlpha = rhs.get_alpha();
        auto subAlpha = std::make_shared<M>(1, lhsAlpha->cols() + rhsAlpha->cols());
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (long i = 0; i < lhsAlpha->cols() + rhsAlpha->cols(); i++) {
            (*subAlpha)(0, i) = i < lhsAlpha->cols() ? (*lhsAlpha)(0, i) : -(*rhsAlpha)(0, i - lhsAlpha->cols());
        }

        auto lhsEta = lhs.get_eta();
        auto rhsEta = rhs.get_eta();
        auto subEta = std::make_shared<M>(lhsEta->rows() + rhsEta->rows(), 1);
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (long i = 0; i < lhsEta->rows() + rhsEta->rows(); i++) {
            (*subEta)(i, 0) = i < lhsEta->rows() ? (*lhsEta)(i, 0) : (*rhsEta)(i - lhsEta->rows(), 0);
        }

        std::vector<std::shared_ptr<M>> subMu = {};
        std::mutex subMuMutex = std::mutex();
        auto lhsMu = lhs.get_mu();
        auto rhsMu = rhs.get_mu();
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (size_t i = 0; i < subCharacters; i++) {
            auto muX = std::make_shared<M>(MatrixSp(subStates, subStates));
            if (i < lhsMu.size()) {
                (*muX).block(0, 0, lhsStates, lhsStates) = *(lhsMu[i]);
            }
            if (i < rhsMu.size()) {
                (*muX).block(lhsStates, lhsStates, rhsStates, rhsStates) = *(rhsMu[i]);
            }
            std::lock_guard<std::mutex> guard(subMuMutex);
            subMu.push_back(muX);
        }
        return std::make_shared<WeightedAutomatonInstance<M>>(subStates, subCharacters, subAlpha, subMu,
                                                                   subEta);
    }


    const std::string pretty_print() const override {
        std::stringstream result;
        Eigen::IOFormat fmt(Eigen::StreamPrecision, 0, ", ", ";\n", "[", "]", "[", "]");
        int i = 0;

        result << "No. States: " << this->states << " , No. Input Characters " << this->noInputCharacters << std::endl
               << "Initial Vector :" << std::endl
               << Eigen::MatrixXd(*(this->alpha)).format(fmt) << std::endl
               << "Transition Matrices: " << std::endl;
        for (const auto &mat : this->mu) {
            result << "Mu" << i << std::endl << Eigen::MatrixXd(*mat).format(fmt) << std::endl;
            i++;
        }
        result << "Final Vector: " << std::endl
               << Eigen::MatrixXd(*(this->eta)).format(fmt) << std::endl << std::endl;
        return result.str();
    }

    static inline std::vector<Eigen::RowVectorXi> generate_random_vectors(
            std::shared_ptr<WeightedAutomatonInstance<M>> &A, uint K) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<> uniform(1, static_cast<int>(A->get_states() * K));
        std::vector<Eigen::RowVectorXi> randV = {};

        for (uint i = 0; i < A->get_states(); i++) {
            auto vect = Eigen::RowVectorXi(1, A->get_number_input_characters());
            for (uint j = 0; j < A->get_number_input_characters(); j++) {
                vect(j) = uniform(rng);
            }
            randV.push_back(vect);
        }
        return randV;
    }

    static bool equivalent(const WeightedAutomatonInstance<M> &lhs, const WeightedAutomatonInstance<M> &rhs,
            uint K = 300)  {
        if (lhs.get_number_input_characters() != rhs.get_number_input_characters()) {
            return false;
        }
        auto subtractionAutomaton = create_subtraction_automaton(lhs, rhs);

        if (!(*(subtractionAutomaton->get_alpha()) * *(subtractionAutomaton->get_eta())).isZero()) {
            return false;
        }

        auto randomVectors = generate_random_vectors(subtractionAutomaton, K);
        auto v = (*subtractionAutomaton->get_eta());
        auto sMu = subtractionAutomaton->get_mu();
        auto sAlpha = subtractionAutomaton->get_alpha();

        for (size_t i = 0; i < subtractionAutomaton->get_states(); i++) {
            for (uint j = 0; j < sMu.size(); j++) {
                v += (randomVectors[i](j) * *(sMu[j]) * v);
            }
            if (!((*sAlpha * v).isZero())) {
                return false;
            }
        }
        return true;
    }
};

template<Matrix M>
WeightedAutomatonInstance<M>::WeightedAutomatonInstance() {}

template<Matrix M>
WeightedAutomatonInstance<M>::~WeightedAutomatonInstance() {}

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
