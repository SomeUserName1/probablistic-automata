#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H

#include "../../util/ParseUtils.h"
#include "../ModelInterface.h"
#include "../Term.h"
#include "RewriteSystem.h"

class RewriteSystemModel : public ModelInterface {
private:
  std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
  std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;

public:
  RewriteSystemModel() = default;
  ~RewriteSystemModel() override;

  [[nodiscard]] auto get_name() const -> std::string override {
    return "RewriteSystemModel";
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
        name = extract_entity_name(line);
        while (!name.empty()) {
          if (std::find(pMapping.begin(), pMapping.end(), name) ==
              pMapping.end()) {
            pMapping.emplace_back(name);
          }
          name = extract_entity_name(line);
        }
      }
    }

    // Step 2: extract rules
    std::vector<std::shared_ptr<RewriteSystem::Rule>> pRules = {};
    std::vector<std::shared_ptr<Term>> lhs;
    std::vector<std::shared_ptr<Term>> rhs;
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
      rateInput =
          line.substr(end + strlen(","), line.size());
      // - (end + strlen(","))

      lhs = Term::extract_terms(pMapping, lhsInput);
      rhs = Term::extract_terms(pMapping, rhsInput);
      rate = extract_number<double>(rateInput);
      pRules.push_back(std::make_shared<RewriteSystem::Rule>(rate, lhs, rhs));
      line = get_next_line(string, "\n", TrimType::TrimWhiteSpace);
    }
    return std::make_shared<RewriteSystem>(pMapping, pRules);
  }

  [[nodiscard]] auto get_reduction_methods() const
      -> std::vector<std::shared_ptr<ReductionMethodInterface>> override {
    return std::vector<std::shared_ptr<ReductionMethodInterface>>();
  }

  [[nodiscard]] auto get_conversion_methods() const
      -> std::vector<std::shared_ptr<ConversionMethodInterface>> override {
    return std::vector<std::shared_ptr<ConversionMethodInterface>>();
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
