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

bool WeightedAutomatonInstance::operator==(const WeightedAutomatonInstance& lhs, const WeightedAutomatonInstance& rhs)
    const {
    if (lhs.get_number_input_characters() != rhs.get_number_input_characters()) {
        return false;
    }
    auto subtractionAutomaton = create_subtraction_automaton(lsh, rhs);

    if (!(subtractionAutomaton->get_alpha() * subtractionAutomaton->get_eta())->isZero()) {
        return false;
    }

    for (int i = 0; i < subtractionAutomaton->get_states(); i++) {

    }

}

bool WeightedAutomatonInstance::operator!=(const WeightedAutomatonInstance& lhs, const WeightedAutomatonInstance& rhs)
    const {
    return !(lhs == rhs);
}

const std::shared_ptr<WeightedAutomatonInstance>
WeightedAutomatonInstance::create_subtraction_automaton(const WeightedAutomatonInstance& lhs,
        const WeightedAutomatonInstance& rhs) const {
    int i;
    int lhsStates = lhs.get_states();
    int rhsStates = rhs.get_states();
    int states = lhsStates + rhsStates;
    int characters = lhs.get_number_input_characters();

    auto lhsAlpha = lhs.get_alpha();
    auto rhsAlpha = rhs.get_alpha();
    auto alpha = std::make_shared<Eigen::RowVectorXf>(lhsAlpha->cols() + rhsAlpha->cols());
    for (i = 0; i < lhsAlpha->cols() + rhsAlpha->cols(); i++) {
        (*alpha)(1, i) = i < lhsAlpha->cols() ? lhsAlpha(1, i) : - rhsAlpha(1, i);
    }

    auto lhsEta = lhs.get_eta();
    auto rhsEta = rhs.get_eta();
    auto eta = std::make_shared<Eigen::VectorXf>(lhsEta->rows() + rhsEta->rows());
    for (i = 0; i < lhsEta->rows() + rhsEta->rows(); i++) {
        (*eta)(1, i) = i < lhsEta->rows() ? lhsEta(i, 1) : - rhsEta(i, 1);
    }

    std::vector<std::shared_ptr<Eigen::MatrixXf>> mu = {};
    auto lhsMu = lhs.get_mu();
    auto rhsMu = rhs.get_mu();
    for (i = 0; i < characters; i++) {
        auto muX = std::make_shared<Eigen::MatrixXf>(Eigen::MatrixXf::Zero(states, states));
        (*mux).block(0, 0, lhsStates, lhsStates) = *(lhsMu[i]);
        (*muX).block(lhsStates + 1, lhsStates + 1, rhsStates, rhsStates);
        mu.push_back(muX)
    }
    return std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu,
                                                       eta);
}

