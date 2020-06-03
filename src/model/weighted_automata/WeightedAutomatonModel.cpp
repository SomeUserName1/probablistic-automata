#include "WeightedAutomatonModel.h"

std::string WeightedAutomatonModel::get_name() const {
    return "Weighted Automaton Model";
}

std::shared_ptr<RepresentationInterface> WeightedAutomatonModel::validate_model_instance(std::string &str) const {
    auto line = get_next_line(str, true);
    if (line.starts_with("input=dense")) {
        return validate_model_instance_dense(str);
    } else if (line.starts_with("input=sparse")) {
        return validate_model_instance_sparse(str);
    } else {
        throw std::invalid_argument("The input specification must be either using sparse or dense format and declare "
                                    "this in the first line as 'sparse' or 'dense'!");
    }
}

std::shared_ptr<RepresentationInterface> WeightedAutomatonModel::validate_model_instance_dense(std::string &str) {
    auto line = get_next_line(str, true);

    // states
    if (!line.starts_with("states")) {
        throw std::invalid_argument("The states must be specified first!");
    }
    int states = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (states <= 0) {
        throw std::invalid_argument("The number of states must be grater than zero!");
    }

    // fetch next line
    line = get_next_line(str, true);

    // characters
    if (!line.starts_with("characters")) {
        throw std::invalid_argument("The number of characters must be specified second!");
    }
    int characters = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (characters <= 0) {
        throw std::invalid_argument("The number of states must be grater than zero!");
    }

    // fetch next line
    line = get_next_line(str, true);

    // alpha
    if (!line.starts_with("alpha")) {
        throw std::invalid_argument("The initial vector needs to be specified as 3rd!");
    }
    auto alpha = std::make_shared<Eigen::RowVectorXd>(states);
    line = line.substr(line.find('=') + 1, line.size());
    for (int i = 0; i < states; i++) {
        (*alpha)(0, i) = extract_one_digit<double>(line);
    }

    // fetch next line
    line = get_next_line(str, true);

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
                (*muX)(i, j) = extract_one_digit<double>(line);
            }
        }
        mu.push_back(muX);
        // fetch next line
        line = get_next_line(str, true);
    }

    if (!line.starts_with("eta")) {
        throw std::invalid_argument("Please specify the final state vector last");
    }
    auto eta = std::make_shared<Eigen::VectorXd>(states);
    line = line.substr(line.find("=") + 1, line.size());
    for (int i = 0; i < states; i++) {
        (*eta)(i, 0) = extract_one_digit<double>(line);
    }
    std::cout << "Parsed Automaton successfully" << std::endl;
    return std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
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

std::shared_ptr<RepresentationInterface>
WeightedAutomatonModel::validate_model_instance_sparse(std::string &str) {
    auto line = get_next_line(str, false);

    // states
    if (!line.starts_with("states")) {
        throw std::invalid_argument("The states must be specified first!");
    }
    int states = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (states <= 0) {
        throw std::invalid_argument("The number of states must be grater than zero!");
    }

    // fetch next line
    line = get_next_line(str, false);

    // characters
    if (!line.starts_with("characters")) {
        throw std::invalid_argument("The number of characters must be specified second!");
    }
    int characters = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (characters <= 0) {
        throw std::invalid_argument("The number of states must be grater than zero!");
    }

    // fetch next line
    line = get_next_line(str, false);

    // alpha
    if (!line.starts_with("alpha")) {
        throw std::invalid_argument("The initial vector needs to be specified as 3rd!");
    }
    auto alpha = std::make_shared<Eigen::RowVectorXd>(states);
    line = line.substr(line.find(':') + 1, line.size());
    (*alpha)(0, extract_one_digit<uint>(line)) = extract_one_digit<double>(line);


    // fetch next line
    line = get_next_line(str, false);

    // mu
    std::vector<std::shared_ptr<Eigen::MatrixXd>> mu = {};
    if (!line.starts_with("mu")) {
        throw std::invalid_argument("Specify the transition matrices 4th, one for each character!");
    }
    line = line.substr(line.find(':') + 1, line.size());
    for (int i = 0; i < characters; i++) {
        mu.emplace_back(std::make_shared<Eigen::MatrixXd>(Eigen::MatrixXd::Zero(states, states)));
    }
    size_t a;
    long b, c;
    double d;
    do {
        a = extract_one_digit<size_t>(line);
        b = extract_one_digit<long>(line);
        c = extract_one_digit<long>(line);
        d = extract_one_digit<double>(line);
        (*(mu[a]))(b, c) = d;

        // fetch next line
        line = get_next_line(str, false);
    } while (!line.starts_with("eta") && !line.empty());

    if (!line.starts_with("eta")) {
        throw std::logic_error("Unreachable;");
    }
    auto eta = std::make_shared<Eigen::VectorXd>(states);
    line = line.substr(line.find("eta=") + 4, line.size());
    (*eta)(extract_one_digit<uint>(line), 0) = extract_one_digit<double>(line);
    std::cout << "Parsed Automaton successfully" << std::endl;
    return std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
}

