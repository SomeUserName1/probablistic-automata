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

        void set_states(int newStates) {
            this->states = newStates;
        }
        int get_number_input_characters() const {
            return noInputCharacters;
        }

        void set_number_input_characters(int newNoInputCharacters) {
            this->noInputCharacters = newNoInputCharacters;
        }

        [[nodiscard]] const std::shared_ptr<Eigen::Matrix<float, 1, Eigen::Dynamic>> &get_alpha() const {
            return alpha;
        }

        void set_alpha(const std::shared_ptr<Eigen::Matrix<float, 1, Eigen::Dynamic>> &newAlpha) {
            this->alpha = newAlpha;
        }

        [[nodiscard]] const std::vector<std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>>
                &get_mu() const {
            return mu;
        }

        void set_mu(const std::vector<std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>> &newMu) {
            this->mu = newMu;
        }

        [[nodiscard]] const std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, 1>> &get_eta() const {
            return eta;
        }

        void set_eta(const std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, 1>> &newEta) {
            this->eta = newEta;
        }

        WeightedAutomatonInstance(int states, int characters, std::shared_ptr<Eigen::Matrix<float, 1, Eigen::Dynamic>>
        alpha, std::vector<std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>> mu,
                std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, 1>> eta)
                : states(states), noInputCharacters(characters), alpha(std::move(alpha)), mu(std::move(mu)),
                eta(std::move(eta)) {}

        std::string get_representation_description() const override {
            return "You need to specify 5 variables: \n "
                   "The number of states by: states=\n"
                   "The number of characters in the input alphabet: characters=\n"
                   "The initial vector: alpha="
                   "The transition matrices (one per input character): muX=\n"
                   "The final vector: eta=\n\n"
                   "Example:\n"
                   "states=4\n"
                   "characters=2\n"
                   "alpha=(1,0,0,0)"
                   "mu1=((0,1,1,0),(0,0,0,0),(0,0,0,0),(0,0,0,0))"
                   "mu2=((0,0,0,0),(0,0,0,1),(0,0,0,1),(0,0,0,0))"
                   "eta=(0,0,0,1)";
        }

        WeightedAutomatonInstance() = default;
        ~WeightedAutomatonInstance() override = default;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
