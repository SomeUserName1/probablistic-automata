#include <utility>

#include <utility>

#include <utility>

#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H

#include <utility>
#include <vector>
#include <Eigen/SparseCore>
#include <memory>

class WeightedAutomatonInstance : public RepresentationInterface {
    private:
        int states{};
        int noInputCharacters{};
        std::shared_ptr<Eigen::Matrix<float, 1, Eigen::Dynamic>> alpha;
        std::vector<std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>> mu;
        std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, 1>> eta;

    public:
        [[nodiscard]] int get_states() const {
            return states;
        }

        int get_number_input_characters() const {
            return noInputCharacters;
        }

        [[nodiscard]] const std::shared_ptr<Eigen::Matrix<float, 1, Eigen::Dynamic>> &get_alpha() const {
            return alpha;
        }

        [[nodiscard]] const std::vector<std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>>
                &get_mu() const {
            return mu;
        }

        [[nodiscard]] const std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, 1>> &get_eta() const {
            return eta;
        }

        WeightedAutomatonInstance(int states, int characters, std::shared_ptr<Eigen::Matrix<float, 1, Eigen::Dynamic>>
        alpha, std::vector<std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>> mu,
                std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, 1>> eta)
                : states(states), noInputCharacters(characters), alpha(std::move(alpha)), mu(std::move(mu)),
                eta(std::move(eta)) {}

        WeightedAutomatonInstance() = default;
        ~WeightedAutomatonInstance() override = default;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
