#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H

#include "../ModelInterface.h"
#include "../RepresentationInterface.h"

class DifferentialEquationModel : public ModelInterface {
public:
  DifferentialEquationModel() = default;

  ~DifferentialEquationModel() override;

  [[nodiscard]] auto get_name() const -> std::string override {
    return "Differential Equation Model";
  }

  auto
  validate_model_instance(std::string &string) -> std::shared_ptr<RepresentationInterface> override {
    throw NotImplementedException();
  }

  auto summarize_reduction(
      std::shared_ptr<RepresentationInterface> &ptr,
      std::shared_ptr<RepresentationInterface> &sharedPtr) const -> std::string override {

    throw NotImplementedException();
  }

  auto
  get_reduction_methods() const -> std::vector<std::shared_ptr<ReductionMethodInterface>> override {
    throw NotImplementedException();
  }

  auto
  get_conversion_methods() const -> std::vector<std::shared_ptr<ConversionMethodInterface>> override {
    throw NotImplementedException();
  }

  auto get_representation_description() const noexcept -> std::string override {
    return "TODO";
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H
