#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H

#include <utility>
#include <vector>
#include <Eigen/Eigen>
#include <memory>

#include <RepresentationInterface.h>

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

        [[nodiscard]] int get_states() const;

        int get_number_input_characters() const;

        [[nodiscard]] const std::shared_ptr<Eigen::RowVectorXf> &get_alpha() const;

        [[nodiscard]] const std::vector<std::shared_ptr<Eigen::MatrixXf>> &get_mu() const;

        [[nodiscard]] const std::shared_ptr<Eigen::VectorXf> &get_eta() const;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
