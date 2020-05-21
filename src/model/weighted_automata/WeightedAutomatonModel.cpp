#include "WeightedAutomatonModel.h"

#include "KieferSchuetzenbergerReduction.h"
#include "WeightedAutomatonInstance.h"
#include "../../NotImplementedException.h"

std::string WeightedAutomatonModel::get_name() const {
    return "Weighted Automaton Model";
}

std::shared_ptr<RepresentationInterface> WeightedAutomatonModel::validate_model_instance(const std::string &str) const {
    std::string line;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    int states = -1;
    int characters = -1;
    auto alpha = std::make_shared<Eigen::RowVectorXf>();
    std::vector<std::shared_ptr<Eigen::MatrixXf>> mu;
    auto eta = std::make_shared<Eigen::VectorXf>();

    while ((pos = str.find('\n', prev)) != std::string::npos) {
        line = str.substr(prev, pos - prev);
        line.erase(remove(line.begin(), line.end(), ' '));

        if (line.starts_with("states")) {
            states = std::stoi(line.substr(line.find("states="), line.size() - 1));
            if (states <= 0) {
                throw std::invalid_argument("The number of states must be grater than zero!");
            }
        } else if (line.starts_with("characters")) {
            characters = std::stoi(line.substr(line.find("characters="), line.size() - 1));
            if (characters <= 0) {
                throw std::invalid_argument("The number of states must be grater than zero!");
            }
        } else if (line.starts_with("alpha")) {
            if (states == -1) {
                throw std::invalid_argument("Please specify the number of states first");
            }
            std::tuple<float, std::string> digitVectorTuple;

            std::string vector = line.substr(line.find("alpha="), line.size() - 1);

            for (int i = 0; i < states; i++) {
                digitVectorTuple = extract_one_digit(vector);
                (*alpha)(0, i) = std::get<0>(digitVectorTuple);
                vector = std::get<1>(digitVectorTuple);
            }
            if (alpha->cols() != states) {
                throw std::invalid_argument("The initial vector alpha needs to have as much columns as states in the "
                                            "Automaton!");
            }
        } else if (line.starts_with("mu")) {
            if (states == -1) {
                throw std::invalid_argument("Please specify the number of states first");
            }
            std::tuple<float, std::string> digitVectorTuple;
            std::shared_ptr<Eigen::MatrixXf> muX;

            std::string vector = line.substr(line.find("mu="), line.size() - 1);
            for(int i = 0; i < states; i++) {
                for (int j = 0; j < states; j++) {
                    digitVectorTuple = extract_one_digit(vector);
                    (*muX)(i, j) = std::get<0>(digitVectorTuple);
                    vector = std::get<1>(digitVectorTuple);
                }
            }
            if (muX->rows() != muX->cols() || muX->rows() != states) {
                throw std::invalid_argument("The transition matrices need to be square matrices with one row/column for "
                                            "each state!");
            }
            mu.push_back(muX);
        } else if (line.starts_with("eta")) {
            if (states == -1) {
                throw std::invalid_argument("Please specify the number of states first");
            }
            std::tuple<float, std::string> digitVectorTuple;
            std::string vector = line.substr(line.find("eta="), line.size() - 1);

            for (int i = 0; i < states; i++) {
                digitVectorTuple = extract_one_digit(vector);
                (*eta)(i, 0) = std::get<0>(digitVectorTuple);
                vector = std::get<1>(digitVectorTuple);
            }
            if (eta->rows() != states) {
                throw std::invalid_argument("The final vector eta needs to have as much rows as states in the "
                                            "Automaton!");
            }
        } else {
            throw std::invalid_argument("The automaton you specified does not conform with the expected input format!");
        }
        prev = pos + 2;
    }
    if ((int)mu.size() != characters) {
        throw std::invalid_argument("You must specify one transition matrix muX for each character in the input "
                                    "alphabet!");
    }

    return std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
}

std::tuple<float, std::string> WeightedAutomatonModel::extract_one_digit(std::string vector) const {
    bool prevDigit = false;
    uint firstDigit = -1;
    for (uint i = 0; i < vector.size(); i++) {
        if (std::isdigit(vector[i]) && !prevDigit) {
            prevDigit = true;
            firstDigit = i;
        } else if (!std::isdigit(vector[i] && prevDigit)) {
            return std::make_tuple(std::stof(vector.substr(firstDigit, i - 1)), vector.substr(i, vector.size() - 1));
        }
    }
    throw std::invalid_argument("No numbers found in the input!");
}

std::string WeightedAutomatonModel::summarize_reduction(std::shared_ptr<RepresentationInterface> &anInterface,
        std::shared_ptr<RepresentationInterface> &representationInterface) const {
    throw NotImplementedException();
}

std::vector<std::shared_ptr<ReductionMethodInterface>> WeightedAutomatonModel::get_reduction_methods() const {
    return this->reductionMethods;
}

std::vector<std::shared_ptr<ConversionMethodInterface>> WeightedAutomatonModel::get_conversion_methods() const {
    return this->conversionMethods;
}

