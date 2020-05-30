#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H

#include <utility>
#include <vector>
#include <Eigen/Eigen>
#include <memory>
#include <iostream>
#include <sstream>
#include <random>

#include "../RepresentationInterface.h"

class WeightedAutomatonInstance : public RepresentationInterface {
private:
    int states{};
    int noInputCharacters{};
    std::shared_ptr<Eigen::RowVectorXd> alpha;
    std::vector<std::shared_ptr<Eigen::MatrixXd>> mu;
    std::shared_ptr<Eigen::VectorXd> eta;

public:
    WeightedAutomatonInstance(int states, int characters, std::shared_ptr<Eigen::RowVectorXd> alpha,
                              std::vector<std::shared_ptr<Eigen::MatrixXd>> mu, std::shared_ptr<Eigen::VectorXd> eta);

    WeightedAutomatonInstance() = default;

    ~WeightedAutomatonInstance() override = default;

    bool operator==(const WeightedAutomatonInstance &other) const;

    bool operator!=(const WeightedAutomatonInstance &other) const;

    [[nodiscard]] int get_states() const;

    int get_number_input_characters() const;

    [[nodiscard]] const std::shared_ptr<Eigen::RowVectorXd> &get_alpha() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Eigen::MatrixXd>> &get_mu() const;

    [[nodiscard]] const std::shared_ptr<Eigen::VectorXd> &get_eta() const;

    static const std::shared_ptr<WeightedAutomatonInstance>
    create_subtraction_automaton(const WeightedAutomatonInstance &lhs, const WeightedAutomatonInstance &rhs);

    const std::string pretty_print() const override;

    static inline std::vector<Eigen::RowVectorXi> generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &,
                                                                          int);

    static bool equivalent(const WeightedAutomatonInstance &, const WeightedAutomatonInstance &, int k = 300);
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
