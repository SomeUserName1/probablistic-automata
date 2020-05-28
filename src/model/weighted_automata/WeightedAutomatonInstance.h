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
        std::shared_ptr<Eigen::RowVectorXf> alpha;
        std::vector<std::shared_ptr<Eigen::MatrixXf>> mu;
        std::shared_ptr<Eigen::VectorXf> eta;

    public:
        WeightedAutomatonInstance(int states, int characters, std::shared_ptr<Eigen::RowVectorXf> alpha,
                std::vector<std::shared_ptr<Eigen::MatrixXf>> mu, std::shared_ptr<Eigen::VectorXf> eta);

        WeightedAutomatonInstance() = default;
        ~WeightedAutomatonInstance() override = default;

        bool operator==(const WeightedAutomatonInstance& other) const;

        bool operator!=(const WeightedAutomatonInstance& other) const;

        [[nodiscard]] int get_states() const;

        int get_number_input_characters() const;

        [[nodiscard]] const std::shared_ptr<Eigen::RowVectorXf> &get_alpha() const;

        [[nodiscard]] const std::vector<std::shared_ptr<Eigen::MatrixXf>> &get_mu() const;

        [[nodiscard]] const std::shared_ptr<Eigen::VectorXf> &get_eta() const;

        const std::shared_ptr<WeightedAutomatonInstance> create_subtraction_automaton(const WeightedAutomatonInstance&
        lhs, const WeightedAutomatonInstance& rhs) const;

        const std::string pretty_print() const override;

        inline std::vector<Eigen::RowVectorXi> generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &,
                int) const;

    bool equivalent(const WeightedAutomatonInstance&, const WeightedAutomatonInstance&, int k=300) const;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
