#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H

#include <utility>

#include "../../util/ParseUtils.h"
#include "../ModelInterface.h"
#include "KieferSchuetzenbergerReduction.h"
#include "WeightedAutomaton.h"
#include "WeightedAutomatonBenchmarks.h"

class WeightedAutomatonModel : public ModelInterface {
private:
  std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
  std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;

public:
  WeightedAutomatonModel()
      : reductionMethods(
            {std::make_shared<KieferSchuetzenbergerReduction<MatDenD>>()}),
        conversionMethods({}) {}

  ~WeightedAutomatonModel() override;

  [[nodiscard]] auto get_name() const -> std::string override {
    return "Weighted Automaton Model";
  }

  auto parse(std::string &str)
      -> std::shared_ptr<RepresentationInterface> override {
    auto line = get_next_line(str, ";", TrimWhiteSpace);
    if (!line.starts_with("input=dense")) {
      if (line.starts_with("input=sparse")) {
        this->reductionMethods = {
            std::make_shared<KieferSchuetzenbergerReduction<MatSpD>>()};
        return validate_model_instance_sparse(str);
      }
      throw std::invalid_argument(
          "The input specification must be either using sparse or dense "
          "format and declare "
          "this in the first line as 'sparse' or 'dense'!");
    }
    return validate_model_instance_dense(str);
  }

  static auto validate_model_instance_dense(std::string &str)
      -> std::shared_ptr<RepresentationInterface> {
    auto line = get_next_line(str, ";", TrimWhiteSpace);

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
    line = get_next_line(str, ";", TrimWhiteSpace);

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
    line = get_next_line(str, ";", TrimWhiteSpace);

    // alpha
    if (!line.starts_with("alpha")) {
      throw std::invalid_argument(
          "The initial vector needs to be specified as 3rd!");
    }
    MatDenDPtr alpha = std::make_shared<MatDenD>(1, states);
    line = line.substr(line.find('=') + 1, line.size());
    for (int i = 0; i < states; i++) {
      alpha->coeffRef(0, i) = extract_number<double>(line);
    }

    // fetch next line
    line = get_next_line(str, ";", TrimWhiteSpace);

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
          muX->coeffRef(i, j) = extract_number<double>(line);
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
    line = get_next_line(str, ";", TrimWhiteSpace);

    if (!line.starts_with("eta")) {
      throw std::invalid_argument("Please specify the final state vector last");
    }
    MatDenDPtr eta = std::make_shared<MatDenD>(states, 1);
    line = line.substr(line.find('=') + 1, line.size());
    for (int i = 0; i < states; i++) {
      eta->coeffRef(i, 0) = extract_number<double>(line);
    }
    std::cout << "Parsed Automaton successfully" << std::endl;
    return std::make_shared<WeightedAutomaton<MatDenD>>(states, characters,
                                                        alpha, mu, eta);
  }

  static auto validate_model_instance_sparse(std::string &str)
      -> std::shared_ptr<RepresentationInterface> {
    size_t a = 0;
    long b = 0;
    long c = 0;
    double d = 0.0;
    auto line = get_next_line(str, ";", TrimNewLines);

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
    line = get_next_line(str, ";", TrimNewLines);

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
    line = get_next_line(str, ";", TrimNewLines);

    // alpha
    if (!line.starts_with("alpha")) {
      throw std::invalid_argument(
          "The initial vector needs to be specified as 3rd!");
    }
    auto alpha = std::make_shared<MatSpD>(1, states);
    line = line.substr(line.find(':') + 1, line.size());
    b = extract_number<uint>(line);
    d = extract_number<double>(line);
    alpha->coeffRef(0, b) = d;

    // fetch next line
    line = get_next_line(str, ";", TrimNewLines);

    // mu
    std::vector<std::shared_ptr<MatSpD>> mu = {};
    if (!line.starts_with("mu")) {
      throw std::invalid_argument(
          "Specify the transition matrices 4th, one for each character!");
    }
    line = line.substr(line.find(':') + 1, line.size());
    for (int i = 0; i < characters; i++) {
      mu.emplace_back(std::make_shared<MatSpD>(states, states));
    }

    do {
      a = extract_number<size_t>(line);
      b = extract_number<long>(line);
      c = extract_number<long>(line);
      d = extract_number<double>(line);
      mu[a]->coeffRef(b, c) = d;

      // fetch next line
      line = get_next_line(str, ";", TrimNewLines);
    } while (!line.starts_with("eta") && !line.empty());

    if (!line.starts_with("eta")) {
      throw std::logic_error("Unreachable;");
    }
    auto eta = std::make_shared<MatSpD>(states, 1);
    line = line.substr(line.find(':') + 1, line.size());
    b = extract_number<uint>(line);
    d = extract_number<double>(line);
    eta->coeffRef(b, 0) = d;
    std::cout << "Parsed Automaton successfully" << std::endl;

    return std::make_shared<WeightedAutomaton<MatSpD>>(states, characters,
                                                       alpha, mu, eta);
  }

  [[nodiscard]] auto get_reduction_methods() const
      -> std::vector<std::shared_ptr<ReductionMethodInterface>> override {
    return this->reductionMethods;
  }

  [[nodiscard]] auto get_conversion_methods() const
      -> std::vector<std::shared_ptr<ConversionMethodInterface>> override {
    return this->conversionMethods;
  }

  [[nodiscard]] auto get_representation_description() const noexcept
      -> std::string override {
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
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
