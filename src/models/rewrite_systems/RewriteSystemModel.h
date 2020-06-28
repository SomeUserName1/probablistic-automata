#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H

#include "../../util/ParseUtils.h"
#include "../ModelInterface.h"
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
    std::vector<std::string> mapping = {};
    std::istringstream f(string);
    std::string line;
    std::string name;

    while (std::getline(f, line)) {
      if (line.find("->") != std::string::npos) {
        name = extract_entity_name(line);
        while (!name.empty()) {
          if (std::find(mapping.begin(), mapping.end(), name) ==
              mapping.end()) {
            mapping.emplace_back(name);
          }
          name = extract_entity_name(line);
        }
      }
    }

    // Step 2: extract rules
    std::vector<std::shared_ptr<RewriteSystem::Rule>> rules = {};
    auto rs = std::make_shared<RewriteSystem>(mapping, rules);

    std::vector<std::shared_ptr<RewriteSystem::Term>> lhs;
    std::vector<std::shared_ptr<RewriteSystem::Term>> rhs;
    std::string lhsInput;
    std::string rhsInput;
    std::string rateInput;
    size_t middle;
    size_t end;
    double rate;
    line = get_next_line(string, "\n", TrimType::TrimWhiteSpace);
    while (!line.empty()) {
      middle = line.find("->");
      end = line.find(",");

      lhsInput = line.substr(0, middle - 0);
      rhsInput =
          line.substr(middle + sizeof("->"), end - (middle + sizeof("->")));
      rateInput =
          line.substr(end + sizeof(","), line.size() - (end + sizeof(",")));

      lhs = extract_terms(*rs, lhsInput);
      rhs = extract_terms(*rs, rhsInput);
      rate = extract_number<double>(rateInput);
      rules.push_back(std::make_shared<RewriteSystem::Rule>(*rs, rate, lhs, rhs));
    }
    return std::move(rs);
  }

  static inline auto extract_term(RewriteSystem &parent, std::string &input)
      -> std::shared_ptr<RewriteSystem::Term> {
    // TODO continue here
    // peek next symbol & use extrat number + extract entity name
  }

  static inline auto extract_terms(RewriteSystem &parent, std::string &input)
      -> std::vector<std::shared_ptr<RewriteSystem::Term>> {
    std::vector<std::shared_ptr<RewriteSystem::Term>> result = {};
    std::string term;
    size_t pos = input.find("+");
    size_t prev = 0;
    while (pos != std::string::npos) {
      term = input.substr(prev, pos - prev);
      result.emplace_back(extract_term(parent, term));
      prev = pos + sizeof("+");
      pos = input.find("+");
    }
    return result;
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
