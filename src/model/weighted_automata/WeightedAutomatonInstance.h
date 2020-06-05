#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H

#include <utility>
#include <vector>
#include <eigen3/Eigen/Eigen>
#include <memory>
#include <iostream>
#include <sstream>
#include <random>
#include <mutex>
#include <variant>

#include "../RepresentationInterface.h"

class WeightedAutomatonInstance : public RepresentationInterface {
private:
    uint states{};
    uint noInputCharacters{};
    std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>, Eigen::RowVectorXd>> alpha;
    std::vector<std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>, Eigen::MatrixXd>>> mu;
    std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>, Eigen::VectorXd>> eta;

public:
    WeightedAutomatonInstance(uint states, uint characters,
        std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>, Eigen::RowVectorXd>> alpha,
        std::vector<std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>,
                Eigen::MatrixXd>>> mu,
        std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>, Eigen::VectorXd>> eta);


    WeightedAutomatonInstance() = default;

    ~WeightedAutomatonInstance() override = default;

    bool operator==(const WeightedAutomatonInstance &other) const;

    bool operator!=(const WeightedAutomatonInstance &other) const;

    double process_word(const std::vector<uint> &word) const;

    [[nodiscard]] uint get_states() const;

    uint get_number_input_characters() const;

    [[nodiscard]] const std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>,
    Eigen::RowVectorXd>>
        &get_alpha() const;

    [[nodiscard]] const std::vector<std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>,
        Eigen::MatrixXd>>> &get_mu() const;

    [[nodiscard]] const std::shared_ptr<std::variant<Eigen::SparseMatrix<double, Eigen::ColMajor, long>, Eigen::VectorXd>>
        &get_eta() const;

    static const std::shared_ptr<WeightedAutomatonInstance>
    create_subtraction_automaton(const WeightedAutomatonInstance &lhs, const WeightedAutomatonInstance &rhs);

    const std::string pretty_print() const override;

    static inline std::vector<Eigen::RowVectorXi> generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                                          uint K) {
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

    static bool equivalent(const WeightedAutomatonInstance &, const WeightedAutomatonInstance &, uint k = 300);
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
