#include "WeightedAutomatonModel.h"

std::string WeightedAutomatonModel::get_name() const {
    return "Weighted Automaton Model";
}

std::shared_ptr<RepresentationInterface> WeightedAutomatonModel::validate_model_instance(const std::string &str) const {
    auto[line, prev] = get_next_line(str, 0);

    // states
    if (!line.starts_with("states")) {
        throw std::invalid_argument("The states must be specified first!");
    }
    int states = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (states <= 0) {
        throw std::invalid_argument("The number of states must be grater than zero!");
    }

    // fetch next line
    std::tie(line, prev) = get_next_line(str, prev);

    // characters
    if (!line.starts_with("characters")) {
        throw std::invalid_argument("The number of characters must be specified second!");
    }
    int characters = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (characters <= 0) {
        throw std::invalid_argument("The number of states must be grater than zero!");
    }

    // fetch next line
    std::tie(line, prev) = get_next_line(str, prev);

    // alpha
    if (!line.starts_with("alpha")) {
        throw std::invalid_argument("The initial vector needs to be specified as 3rd!");
    }
    auto alpha = std::make_shared<Eigen::RowVectorXd>(states);
    line = line.substr(line.find('=') + 1, line.size());
    for (int i = 0; i < states; i++) {
        std::tie((*alpha)(0, i), line) = extract_one_digit(line);
    }

    // fetch next line
    std::tie(line, prev) = get_next_line(str, prev);

    // mu
    std::vector<std::shared_ptr<Eigen::MatrixXd>> mu = {};
    for (int k = 0; k < characters; k++) {
        if (!line.starts_with("mu")) {
            throw std::invalid_argument("Specify the transition matrices 4th, one for each character!");
        }
        std::shared_ptr<Eigen::MatrixXd> muX = std::make_shared<Eigen::MatrixXd>(states, states);
        line = line.substr(line.find('=') + 1, line.size());
        for (int i = 0; i < states; i++) {
            for (int j = 0; j < states; j++) {
                std::tie((*muX)(i, j), line) = extract_one_digit(line);
            }
        }
        mu.push_back(muX);
        // fetch next line
        std::tie(line, prev) = get_next_line(str, prev);
    }

    if (!line.starts_with("eta")) {
        throw std::invalid_argument("Please specify the final state vector last");
    }
    auto eta = std::make_shared<Eigen::VectorXd>(states);
    line = line.substr(line.find("eta=") + 4, line.size());
    for (int i = 0; i < states; i++) {
        std::tie((*eta)(i, 0), line) = extract_one_digit(line);
    }
    std::cout << "Parsed Automaton successfully" << std::endl;
    return std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
}

std::tuple<std::string, std::size_t> WeightedAutomatonModel::get_next_line(const std::string &str, std::size_t
prev) noexcept {
    std::string line;
    std::size_t pos = str.find(';', prev);
    line = str.substr(prev, pos - prev);
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
    prev = pos + 1;
    return std::make_tuple(line, prev);
}

inline std::tuple<float, std::string> WeightedAutomatonModel::extract_one_digit(const std::string &vector) {
    bool prevDigit = false;
    int firstDigit = -1;
    for (int i = 0; i < (int) vector.size(); i++) {
        if (std::isdigit(vector[i]) && !prevDigit) {
            prevDigit = true;
            firstDigit = i;
        } else if (!std::isdigit(vector[i]) && prevDigit) {
            return {std::stof(vector.substr(firstDigit, i - 1)), vector.substr(i, vector.size())};
        }
    }
    throw std::invalid_argument("No numbers found in the input!");
}

std::string WeightedAutomatonModel::summarize_reduction(std::shared_ptr<RepresentationInterface> &A,
                                                        std::shared_ptr<RepresentationInterface> &minA) const {
    std::stringstream result;
    auto WA = std::dynamic_pointer_cast<WeightedAutomatonInstance>(A);
    auto minWA = std::dynamic_pointer_cast<WeightedAutomatonInstance>(minA);
    result << "Before Reduction" << std::endl
           << WA->pretty_print()
           << "After Reduction" << std::endl
           << minWA->pretty_print();
    return result.str();
}

std::vector<std::shared_ptr<ReductionMethodInterface>> WeightedAutomatonModel::get_reduction_methods() const {
    return this->reductionMethods;
}

std::vector<std::shared_ptr<ConversionMethodInterface>> WeightedAutomatonModel::get_conversion_methods() const {
    return this->conversionMethods;
}

