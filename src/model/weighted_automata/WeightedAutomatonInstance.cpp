#include "WeightedAutomatonInstance.h"


WeightedAutomatonInstance::WeightedAutomatonInstance(int states, int characters,
                                                     std::shared_ptr<Eigen::RowVectorXf> alpha,
                                                     std::vector<std::shared_ptr<Eigen::MatrixXf>> mu,
                                                     std::shared_ptr<Eigen::VectorXf> eta)
     : states(states), noInputCharacters(characters), alpha(std::move(alpha)), mu(std::move(mu)), eta(std::move(eta)) {}

int WeightedAutomatonInstance::get_states() const {
    return this->states;
}

int WeightedAutomatonInstance::get_number_input_characters() const {
    return this->noInputCharacters;
}

const std::shared_ptr<Eigen::RowVectorXf> &WeightedAutomatonInstance::get_alpha() const {
    return this->alpha;
}

const std::vector<std::shared_ptr<Eigen::MatrixXf>> &
WeightedAutomatonInstance::get_mu() const {
    return this->mu;
}

const std::shared_ptr<Eigen::VectorXf> &WeightedAutomatonInstance::get_eta() const {
    return this->eta;
}

const std::string WeightedAutomatonInstance::pretty_print() const {
    std::stringstream result;
    Eigen::IOFormat fmt(Eigen::StreamPrecision, 0, ", ", ";\n", "[", "]", "[", "]");
    int i = 0;

    result << "No. States: " << this->states << " , No. Input Characters " << this->noInputCharacters << std::endl
           << "Initial Vector :" << std::endl
           << this->alpha->format(fmt) << std::endl
           << "Transition Matrices: " << std::endl;
    for (const auto& mat : this->mu) {
        result << "Mu" << i << std::endl << mat->format(fmt) << std::endl;
    }
    result << "Final Vector: " << std::endl
              << this->eta->format(fmt) << std::endl << std::endl;
    return result.str();
}


