#include "WeightedAutomatonInstance.h"


WeightedAutomatonInstance::WeightedAutomatonInstance(uint mStates, uint mCharacters,
                                                     std::shared_ptr<Eigen::RowVectorXd> mAlpha,
                                                     std::vector<std::shared_ptr<Eigen::MatrixXd>> mMu,
                                                     std::shared_ptr<Eigen::VectorXd> mEta)
        : states(mStates), noInputCharacters(mCharacters), alpha(std::move(mAlpha)), mu(std::move(mMu)),
          eta(std::move(mEta)) {}

double WeightedAutomatonInstance::process_word(const std::vector<uint> &word) const {
    auto intermediate = Eigen::RowVectorXd(*(this->alpha));
    for (const auto &letter : word) {
        if (letter >= this->noInputCharacters) {
            throw std::invalid_argument("The specified word has a letter that is not in the input alphabet!");
        }
        intermediate *= *(this->mu[letter]);
    }
    return static_cast<double>(intermediate * *(this->eta));

}

uint WeightedAutomatonInstance::get_states() const {
    return this->states;
}

uint WeightedAutomatonInstance::get_number_input_characters() const {
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
rhs, uint K) {
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

    for (size_t i = 0; i < subtractionAutomaton->get_states(); i++) {
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
    uint lhsStates = lhs.get_states();
    uint rhsStates = rhs.get_states();
    uint subStates = lhsStates + rhsStates;
    uint subCharacters = std::max(lhs.get_number_input_characters(), rhs.get_number_input_characters());

    auto lhsAlpha = lhs.get_alpha();
    auto rhsAlpha = rhs.get_alpha();
    auto subAlpha = std::make_shared<Eigen::RowVectorXd>(lhsAlpha->cols() + rhsAlpha->cols());
    for (long i = 0; i < lhsAlpha->cols() + rhsAlpha->cols(); i++) {
        (*subAlpha)(0, i) = i < lhsAlpha->cols() ? (*lhsAlpha)(0, i) : -(*rhsAlpha)(0, i - lhsAlpha->cols());
    }

    auto lhsEta = lhs.get_eta();
    auto rhsEta = rhs.get_eta();
    auto subEta = std::make_shared<Eigen::VectorXd>(lhsEta->rows() + rhsEta->rows());
    for (long i = 0; i < lhsEta->rows() + rhsEta->rows(); i++) {
        (*subEta)(i, 0) = i < lhsEta->rows() ? (*lhsEta)(i, 0) : (*rhsEta)(i - lhsEta->rows(), 0);
    }

    std::vector<std::shared_ptr<Eigen::MatrixXd>> subMu = {};
    auto lhsMu = lhs.get_mu();
    auto rhsMu = rhs.get_mu();
    for (size_t i = 0; i < subCharacters; i++) {
        auto muX = std::make_shared<Eigen::MatrixXd>(Eigen::MatrixXd::Zero(subStates, subStates));
        if (i < lhsMu.size()) {
            (*muX).block(0, 0, lhsStates, lhsStates) = *(lhsMu[i]);
        }
        if (i < rhsMu.size()) {
            (*muX).block(lhsStates, lhsStates, rhsStates, rhsStates) = *(rhsMu[i]);
        }
        subMu.push_back(muX);
    }
    return std::make_shared<WeightedAutomatonInstance>(subStates, subCharacters, subAlpha, subMu,
                                                       subEta);
}
