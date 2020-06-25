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
    std::vector<std::string> mapping;
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
//    for (const auto &elem : mapping) {
//      std::cout << elem << std::endl;
//    }
//    std::cout << std::endl;

    // Step 2: extract rules
    // TODO continue here
    return std::shared_ptr<RepresentationInterface>();
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
