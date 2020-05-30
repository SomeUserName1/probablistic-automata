#include "WeightedAutomatonInstance.h"


WeightedAutomatonInstance::WeightedAutomatonInstance(int states, int characters,
                                                     std::shared_ptr<Eigen::RowVectorXd> alpha,
                                                     std::vector<std::shared_ptr<Eigen::MatrixXd>> mu,
                                                     std::shared_ptr<Eigen::VectorXd> eta)
        : states(states), noInputCharacters(characters), alpha(std::move(alpha)), mu(std::move(mu)),
          eta(std::move(eta)) {}

int WeightedAutomatonInstance::get_states() const {
    return this->states;
}

int WeightedAutomatonInstance::get_number_input_characters() const {
    return this->noInputCharacters;
}

const std::shared_ptr<Eigen::RowVectorXd> &WeightedAutomatonInstance::get_alpha() const {
    return this->alpha;
}

const std::vector<std::shared_ptr<Eigen::MatrixXd>> &
WeightedAutomatonInstance::get_mu() const {
    return this->mu;
}

const std::shared_ptr<Eigen::VectorXd> &WeightedAutomatonInstance::get_eta() const {
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
    for (const auto &mat : this->mu) {
        result << "Mu" << i << std::endl << mat->format(fmt) << std::endl;
    }
    result << "Final Vector: " << std::endl
           << this->eta->format(fmt) << std::endl << std::endl;
    return result.str();
}

bool WeightedAutomatonInstance::operator==(const WeightedAutomatonInstance &other)
const {
    return equivalent(*this, other);
}

bool WeightedAutomatonInstance::operator!=(const WeightedAutomatonInstance &other)
const {
    return !(*this == other);
}

bool WeightedAutomatonInstance::equivalent(const WeightedAutomatonInstance &lhs, const WeightedAutomatonInstance &
rhs, int K) {
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

    for (int i = 0; i < subtractionAutomaton->get_states(); i++) {
        for (uint j = 0; j < sMu.size(); j++) {
            v = v + (randomVectors[i](j) * *(sMu[j]) * v);
        }
        if (!((*sAlpha * v).isZero())) {
            return false;
        }
    }
    return true;
}

const std::shared_ptr<WeightedAutomatonInstance>
WeightedAutomatonInstance::create_subtraction_automaton(const WeightedAutomatonInstance &lhs,
                                                        const WeightedAutomatonInstance &rhs) {
    int i;
    int lhsStates = lhs.get_states();
    int rhsStates = rhs.get_states();
    int subStates = lhsStates + rhsStates;
    int subCharacters = lhs.get_number_input_characters();

    auto lhsAlpha = lhs.get_alpha();
    auto rhsAlpha = rhs.get_alpha();
    auto subAlpha = std::make_shared<Eigen::RowVectorXd>(lhsAlpha->cols() + rhsAlpha->cols());
    for (i = 0; i < lhsAlpha->cols() + rhsAlpha->cols(); i++) {
        (*subAlpha)(1, i) = i < lhsAlpha->cols() ? (*lhsAlpha)(1, i) : -(*rhsAlpha)(1, i);
    }

    auto lhsEta = lhs.get_eta();
    auto rhsEta = rhs.get_eta();
    auto subEta = std::make_shared<Eigen::VectorXd>(lhsEta->rows() + rhsEta->rows());
    for (i = 0; i < lhsEta->rows() + rhsEta->rows(); i++) {
        (*subEta)(1, i) = i < lhsEta->rows() ? (*lhsEta)(i, 1) : -(*rhsEta)(i, 1);
    }

    std::vector<std::shared_ptr<Eigen::MatrixXd>> subMu = {};
    auto lhsMu = lhs.get_mu();
    auto rhsMu = rhs.get_mu();
    for (i = 0; i < subCharacters; i++) {
        auto muX = std::make_shared<Eigen::MatrixXd>(Eigen::MatrixXd::Zero(subStates, subStates));
        (*muX).block(0, 0, lhsStates, lhsStates) = *(lhsMu[i]);
        (*muX).block(lhsStates + 1, lhsStates + 1, rhsStates, rhsStates);
        subMu.push_back(muX);
    }
    return std::make_shared<WeightedAutomatonInstance>(subStates, subCharacters, subAlpha, subMu,
                                                       subEta);
}

inline std::vector<Eigen::RowVectorXi>
WeightedAutomatonInstance::generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &A, int K) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> uniform(1, A->get_states() * K);
    std::vector<Eigen::RowVectorXi> randV;

    auto vect = Eigen::RowVectorXi(1, A->get_number_input_characters());
    for (int i = 0; i < A->get_states(); i++) {
        for (int j = 0; j < A->get_number_input_characters(); j++) {
            vect(j) = uniform(rng);
        }
        randV.push_back(vect);
    }
    return randV;
}