#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H

#include <utility>

#include "../ModelInterface.h"
#include "KieferSchuetzenbergerReduction.h"
#include "WeightedAutomaton.h"

class WeightedAutomatonModel : public ModelInterface {
private:
  std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
  std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;
  size_t dense = 1;

public:
  WeightedAutomatonModel()
      : reductionMethods({std::make_shared<
            KieferSchuetzenbergerReduction<MatDenD>>()}),
        conversionMethods({}) {}

  ~WeightedAutomatonModel() override;

  [[nodiscard]] auto get_name() const -> std::string override {
    return "Weighted Automaton Model";
  }

  auto validate_model_instance(std::string &str)
      -> std::shared_ptr<RepresentationInterface> override {
    auto line = get_next_line(str, true);
    if (!line.starts_with("input=dense")) {
      if (line.starts_with("input=sparse")) {
        this->dense = 0;
        this->reductionMethods = {
            std::make_shared<KieferSchuetzenbergerReduction<
                Eigen::SparseMatrix<double, 0, long>>>()};
        return validate_model_instance_sparse(str);
      } else {
        throw std::invalid_argument(
            "The input specification must be either using sparse or dense "
            "format and declare "
            "this in the first line as 'sparse' or 'dense'!");
      }
    } else {
      return validate_model_instance_dense(str);
    }
  }

  static auto validate_model_instance_dense(std::string &str)
      -> std::shared_ptr<RepresentationInterface> {
    auto line = get_next_line(str, true);

    // states
    if (!line.starts_with("states")) {
      throw std::invalid_argument("The states must be specified first!");
    }
    int states = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (states <= 0) {
      throw std::invalid_argument(
          "The number of states must be grater than zero!");
    }

    // fetch next line
    line = get_next_line(str, true);

    // characters
    if (!line.starts_with("characters")) {
      throw std::invalid_argument(
          "The number of characters must be specified second!");
    }
    int characters = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (characters <= 0) {
      throw std::invalid_argument(
          "The number of states must be grater than zero!");
    }

    // fetch next line
    line = get_next_line(str, true);

    // alpha
    if (!line.starts_with("alpha")) {
      throw std::invalid_argument(
          "The initial vector needs to be specified as 3rd!");
    }
    MatDenDPtr alpha =
        std::make_shared<MatDenD>(1, states);
    line = line.substr(line.find('=') + 1, line.size());
    for (int i = 0; i < states; i++) {
      alpha->coeffRef(0, i) = extract_one_digit<double>(line);
    }

    // fetch next line
    line = get_next_line(str, true);

    // mu
    std::vector<MatDenDPtr> mu = {};
    if (!line.starts_with("mu=(")) {
      throw std::invalid_argument(
          "Specify the transition matrices 4th, one for each character!");
    }
    line = line.substr(line.find('=') + 1, line.size());

    MatDenDPtr muX;
    do {
      muX = std::make_shared<MatDenD>(states, states);
      for (int i = 0; i < states; i++) {
        for (int j = 0; j < states; j++) {
          muX->coeffRef(i, j) = extract_one_digit<double>(line);
        }
      }
      mu.push_back(muX);

      if (!line.starts_with("))")) {
          std::cout << line << std::endl;
        throw std::invalid_argument("A transition matrix should end here but "
                                    "read something different than ')),(('!");
      }
    } while (!line.starts_with(")));") && !line.empty());

    // fetch next line
    line = get_next_line(str, true);

    if (!line.starts_with("eta")) {
      throw std::invalid_argument("Please specify the final state vector last");
    }
    MatDenDPtr eta =
        std::make_shared<MatDenD>(states, 1);
    line = line.substr(line.find('=') + 1, line.size());
    for (int i = 0; i < states; i++) {
      eta->coeffRef(i, 0) = extract_one_digit<double>(line);
    }
    std::cout << "Parsed Automaton successfully" << std::endl;
    return std::make_shared<WeightedAutomaton<MatDenD>>(
        states, characters, alpha, mu, eta, 1);
  }

  static auto validate_model_instance_sparse(std::string &str)
      -> std::shared_ptr<RepresentationInterface> {
    size_t a = 0;
    long b = 0;
    long c = 0;
    double d = 0.0;
    auto line = get_next_line(str, false);

    // states
    if (!line.starts_with("states")) {
      throw std::invalid_argument("The states must be specified first!");
    }
    int states = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (states <= 0) {
      throw std::invalid_argument(
          "The number of states must be grater than zero!");
    }

    // fetch next line
    line = get_next_line(str, false);

    // characters
    if (!line.starts_with("characters")) {
      throw std::invalid_argument(
          "The number of characters must be specified second!");
    }
    int characters = std::stoi(line.substr(line.find('=') + 1, line.size()));
    if (characters <= 0) {
      throw std::invalid_argument(
          "The number of states must be grater than zero!");
    }

    // fetch next line
    line = get_next_line(str, false);

    // alpha
    if (!line.starts_with("alpha")) {
      throw std::invalid_argument(
          "The initial vector needs to be specified as 3rd!");
    }
    auto alpha =
        std::make_shared<Eigen::SparseMatrix<double, 0, long>>(1, states);
    line = line.substr(line.find(':') + 1, line.size());
    b = extract_one_digit<uint>(line);
    d = extract_one_digit<double>(line);
    alpha->coeffRef(0, b) = d;

    // fetch next line
    line = get_next_line(str, false);

    // mu
    std::vector<std::shared_ptr<Eigen::SparseMatrix<double, 0, long>>> mu = {};
    if (!line.starts_with("mu")) {
      throw std::invalid_argument(
          "Specify the transition matrices 4th, one for each character!");
    }
    line = line.substr(line.find(':') + 1, line.size());
    for (int i = 0; i < characters; i++) {
      mu.emplace_back(std::make_shared<Eigen::SparseMatrix<double, 0, long>>(
          states, states));
    }

    do {
      a = extract_one_digit<size_t>(line);
      b = extract_one_digit<long>(line);
      c = extract_one_digit<long>(line);
      d = extract_one_digit<double>(line);
      mu[a]->coeffRef(b, c) = d;

      // fetch next line
      line = get_next_line(str, false);
    } while (!line.starts_with("eta") && !line.empty());

    if (!line.starts_with("eta")) {
      throw std::logic_error("Unreachable;");
    }
    auto eta =
        std::make_shared<Eigen::SparseMatrix<double, 0, long>>(states, 1);
    line = line.substr(line.find(':') + 1, line.size());
    b = extract_one_digit<uint>(line);
    d = extract_one_digit<double>(line);
    eta->coeffRef(b, 0) = d;
    std::cout << "Parsed Automaton successfully" << std::endl;

    return std::make_shared<
        WeightedAutomaton<Eigen::SparseMatrix<double, 0, long>>>(
        states, characters, alpha, mu, eta, 0);
  }

  [[nodiscard]] std::string summarize_reduction(
      std::shared_ptr<RepresentationInterface> &A,
      std::shared_ptr<RepresentationInterface> &minA) const override {
    std::stringstream result;
    if (dense) {
      auto WA = std::static_pointer_cast<WeightedAutomaton<MatDenD>>(A);
      auto minWA =
          std::static_pointer_cast<WeightedAutomaton<MatDenD>>(minA);
      result << "Before Reduction" << std::endl
             << WA->pretty_print() << "After Reduction" << std::endl
             << minWA->pretty_print();
    } else {
      auto WA = std::static_pointer_cast<
          WeightedAutomaton<Eigen::SparseMatrix<double, 0, long>>>(A);
      auto minWA = std::static_pointer_cast<
          WeightedAutomaton<Eigen::SparseMatrix<double, 0, long>>>(minA);
      result << "Before Reduction" << std::endl
             << WA->pretty_print() << "After Reduction" << std::endl
             << minWA->pretty_print();
    }
    return result.str();
  }

  [[nodiscard]] std::vector<std::shared_ptr<ReductionMethodInterface>>
  get_reduction_methods() const override {
    return this->reductionMethods;
  }

  [[nodiscard]] std::vector<std::shared_ptr<ConversionMethodInterface>>
  get_conversion_methods() const override {
    return this->conversionMethods;
  }

  [[nodiscard]] std::string get_representation_description() const
      noexcept override {
    return "You need to specify one flag and 5 variables: \n "
           "The input type: input"
           "The number of states by: states\n"
           "The number of characters in the input alphabet: characters=\n"
           "The initial vector: alpha \n"
           "The transition matrices (one per input character): muX\n"
           "The final vector: eta\n\n\n"
           "Example for the dense type:\n"
           "input=dense\n"
           "states=4;\n"
           "characters=2;\n"
           "alpha=(1,0,0,0);\n"
           "mu=(\n"
           "  ((0,1,1,0),(0,0,0,0),(0,0,0,0),(0,0,0,0)),\n"
           "  ((0,0,0,0),(0,0,0,1),(0,0,0,1),(0,0,0,0)),\n"
           "  );\n"
           "eta=(0,0,0,1);\n\n\n"
           "Example for the sparse type:\n"
           "input=sparse\n"
           "states=4;\n"
           "characters=2;\n"
           "alpha: 0 1\n"
           "mu\n"
           "0 0 1 1;\n"
           "0 0 2 1;\n"
           "1 1 3 1;\n"
           "1 2 3 1;\n"
           "eta: 3 1;\n";
  }

  inline static std::string get_next_line(std::string &str,
                                          bool trimSpaces) noexcept {
    std::string line;
    std::size_t pos = str.find(';');
    line = str.substr(0, pos + 1);
    str.erase(0, pos + 1);
    if (trimSpaces) {
      line.erase(std::remove_if(line.begin(), line.end(), ::isspace),
                 line.end());
    } else {
      line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    }
    return line;
  }

  template <Arithmetic T>
  static inline T extract_one_digit(std::string &vector) {
    bool prevDigit = false;
    size_t firstDigit = 0;
    for (size_t i = 0; i < vector.size(); i++) {
      if ((std::isdigit(vector[i]) || vector[i] == 'e' || vector[i] == '-' || vector[i] == '+' || vector[i] == '.') && !prevDigit) {
        prevDigit = true;
        firstDigit = i;
      } else if (!(std::isdigit(vector[i]) || vector[i] == 'e' || vector[i] == '-' || vector[i] == '+' || vector[i] == '.') && prevDigit) {
        T result = static_cast<T>(std::stod(vector.substr(firstDigit, i)));
        vector.erase(0, i);
        return result;
      }
    }
    throw std::invalid_argument("No numbers found in the input!");
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
