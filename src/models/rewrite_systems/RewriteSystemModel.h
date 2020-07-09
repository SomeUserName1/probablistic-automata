#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H

#include "../../util/ParseUtils.h"
#include "../ModelInterface.h"
#include "MaximalAggregation.h"
#include "RewriteSystem.h"

class RewriteSystemModel : public ModelInterface {
private:
  std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
  std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;

public:
  RewriteSystemModel()
      : reductionMethods({std::make_shared<MaximalAggregation>()}),
        conversionMethods({}) {}
  ~RewriteSystemModel() override;

  [[nodiscard]] auto get_name() const -> std::string override {
    return "RewriteSystemModel";
  }

  static inline auto
  convert_species_string_vector(const std::vector<std::string> &pMapping,
                                std::string &input) {
    bool seenPrevEntity = false;
    std::string prevEntityStr;
    unsigned int prevEntityInt = 0;
    unsigned char next = 0;
    std::vector<unsigned int> word = std::vector(pMapping.size(), 0u);

    while (!input.empty()) {
      next = static_cast<unsigned char>(input[0]);
      if ((std::isupper(next) != 0)) {
        if (seenPrevEntity) {
          word[prevEntityInt] = 1;
        }
        seenPrevEntity = true;
        prevEntityStr = extract_atomic_name(input);
        prevEntityInt = static_cast<unsigned int>(std::distance(
            pMapping.begin(),
            std::find(pMapping.begin(), pMapping.end(), prevEntityStr)));
      } else if ((std::isdigit(next) != 0) && seenPrevEntity) {
        seenPrevEntity = false;
        word[prevEntityInt] = extract_number<unsigned int>(input);
      }
      if (seenPrevEntity && input.empty()) {
        seenPrevEntity = false;
        word[prevEntityInt] = 1;
      }
    }
    bool isZeroVect = true;
    for (const auto &elem : word) {
      if (elem != 0) {
        isZeroVect = false;
        break;
      }
    }
    if (isZeroVect) {
      word = {};
    }
    return word;
  }

  static inline auto
  extract_term(const std::vector<std::string> &pMapping,
               const std::vector<std::vector<unsigned int>> &pSpeciesList,
               std::string &input) -> std::shared_ptr<RewriteSystem::Term> {

    unsigned int factor = 1;
    std::vector<unsigned int> word = std::vector(pMapping.size(), 0u);
    while (!input.empty()) {
      unsigned char next = static_cast<unsigned char>(input[0]);
      if ((std::isdigit(next) != 0)) {
        factor = extract_number<unsigned int>(input);
      } else if ((std::isupper(next) != 0)) {
        word = convert_species_string_vector(pMapping, input);
      }
    }
    unsigned int theSpecies = 0;
    auto it = std::find(std::begin(pSpeciesList), std::end(pSpeciesList), word);
    if (it != std::end(pSpeciesList)) {
      theSpecies = static_cast<unsigned int>(
          std::distance(std::begin(pSpeciesList), it));
    } else {
      throw std::invalid_argument("Could not find species in species list!");
    }
    return std::make_shared<RewriteSystem::Term>(factor, theSpecies);
  }

  static inline auto
  extract_terms(const std::vector<std::string> &pMapping,
                const std::vector<std::vector<unsigned int>> &pSpeciesList,
                std::string &input)
      -> std::vector<std::shared_ptr<RewriteSystem::Term>> {
    std::vector<std::shared_ptr<RewriteSystem::Term>> result = {};
    std::string term;
    size_t pos = 0;
    size_t prev = 0;
    while (pos != std::string::npos) {
      pos = input.find('+', prev);
      term = input.substr(prev, pos - prev);
      result.emplace_back(extract_term(pMapping, pSpeciesList, term));
      prev = pos + strlen("+");
    }
    return result;
  }

  auto parse(std::string &string)
      -> std::shared_ptr<RepresentationInterface> override {
    // Step 1: Enumerate all distinct "Elements"
    std::vector<std::string> pMapping = {};
    std::istringstream f(string);
    std::string line;
    std::string name;

    while (std::getline(f, line)) {
      if (line.find("->") != std::string::npos) {
        name = extract_atomic_name(line);
        while (!name.empty()) {
          if (std::find(pMapping.begin(), pMapping.end(), name) ==
              pMapping.end()) {
            pMapping.emplace_back(name);
          }
          name = extract_atomic_name(line);
        }
      }
    }

    // FIXME cont here
    // Step 2: extract Species
    f = std::istringstream(string);
    std::vector<std::vector<unsigned int>> pSpeciesList = {};
    std::vector<unsigned int> pSpecies;
    while (std::getline(f, line)) {
      if (line.find("->") != std::string::npos) {
        name = extract_species_name(line);
        pSpecies = convert_species_string_vector(pMapping, name);
        while (!pSpecies.empty()) {
          if (std::find(pSpeciesList.begin(), pSpeciesList.end(), pSpecies) ==
              pSpeciesList.end()) {
            pSpeciesList.emplace_back(pSpecies);
          }
          name = extract_species_name(line);
          pSpecies = convert_species_string_vector(pMapping, name);
        }
      }
    }

    // Step 3: extract rules
    std::vector<std::shared_ptr<RewriteSystem::Rule>> pRules = {};
    std::vector<std::shared_ptr<RewriteSystem::Term>> lhs = {};
    std::vector<std::shared_ptr<RewriteSystem::Term>> rhs;
    std::string lhsInput;
    std::string rhsInput;
    std::string rateInput;
    size_t middle = 0;
    size_t end = 0;
    double rate = 0;
    line = get_next_line(string, "\n", TrimType::TrimWhiteSpace);
    while (!line.empty()) {
      if (line.starts_with("begin") || line.starts_with("end")) {
        line = get_next_line(string, "\n", TrimType::TrimWhiteSpace);
        continue;
      }
      middle = line.find("->");
      end = line.find(',');

      lhsInput = line.substr(0, middle - 0);
      rhsInput =
          line.substr(middle + strlen("->"), end - (middle + strlen("->")));
      rateInput = line.substr(end + strlen(","), line.size());
      // - (end + strlen(","))

      if (!lhsInput.empty()) {
        lhs = extract_terms(pMapping, pSpeciesList, lhsInput);
      }
      if (!rhsInput.empty()) {
        rhs = extract_terms(pMapping, pSpeciesList, rhsInput);
      } else {
        throw std::invalid_argument("Rhs of a rule may not be empty!");
      }
      if (!rateInput.empty()) {
        rate = extract_number<double>(rateInput);
      } else {
        throw std::invalid_argument("rate of a rule may not be empty!");
      }
      pRules.emplace_back(
          std::make_shared<RewriteSystem::Rule>(rate, lhs, rhs));
      line = get_next_line(string, "\n", TrimType::TrimWhiteSpace);
    }
    return std::make_shared<RewriteSystem>(pMapping, pSpeciesList, pRules);
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
    return "Triplets of the form a X + b Y -> c Z, rate where a,b,c natural "
           "numbers and X,Y,Z Species\n"
           "begin reactions\n"
           "  Au     -> Ap\t \t, 1.0\n"
           "  Ap     -> Au\t \t, 2.0\n "
           "  Au + B -> AuB\t \t, 3.0\n"
           "  AuB    -> Au + B\t, 4.0\n"
           "  Ap + B -> ApB\t \t, 3.0\n"
           "  ApB    -> Ap + B \t, 4.0\n"
           "end reactions";
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H
