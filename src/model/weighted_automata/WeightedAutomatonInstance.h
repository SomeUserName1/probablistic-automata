//
// Created by someusername on 19.05.20.
//

#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H

#include <utility>
#include <vector>
#include <Eigen/SparseCore>
#include <memory>

class WeightedAutomatonInstance {
    private:
        int states;
public:
    [[nodiscard]] int getStates() const {
        return states;
    }

    void setStates(int newStates) {
        WeightedAutomatonInstance::states = newStates;
    }

    [[nodiscard]] const std::shared_ptr<Eigen::Matrix<double, 1, Eigen::Dynamic>> &getAlpha() const {
        return alpha;
    }

    void setAlpha(const std::shared_ptr<Eigen::Matrix<double, 1, Eigen::Dynamic>> &newAlpha) {
        WeightedAutomatonInstance::alpha = alpha;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>>> &getMu()
    const {
        return mu;
    }

    void setMu(const std::vector<std::shared_ptr<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>>> &newMu) {
        WeightedAutomatonInstance::mu = mu;
    }

    [[nodiscard]] const std::shared_ptr<Eigen::Matrix<double, Eigen::Dynamic, 1>> &getEta() const {
        return eta;
    }

    void setEta(const std::shared_ptr<Eigen::Matrix<double, Eigen::Dynamic, 1>> &newEta) {
        WeightedAutomatonInstance::eta = eta;
    }

private:
    std::shared_ptr<Eigen::Matrix<double, 1, Eigen::Dynamic>> alpha;
        std::vector<std::shared_ptr<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>>> mu;
        std::shared_ptr<Eigen::Matrix<double, Eigen::Dynamic, 1>> eta;

    public:
        WeightedAutomatonInstance(int states, std::shared_ptr<Eigen::Matrix<double, 1, Eigen::Dynamic>> alpha,
                std::vector<std::shared_ptr<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>>> mu,
                std::shared_ptr<Eigen::Matrix<double, Eigen::Dynamic, 1>> eta) {
            this->states = states;
            this->alpha = std::move(alpha);
            this->mu = std::move(mu);
            this->eta = std::move(eta);
        }
        ~WeightedAutomatonInstance() = default;
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONINSTANCE_H
