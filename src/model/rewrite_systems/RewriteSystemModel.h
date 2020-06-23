#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H

#include "../ModelInterface.h"
#include "../../util/ParseUtils.h"

class RewriteSystemModel : public ModelInterface {
private:
  std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
  std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;

public:
  RewriteSystemModel() = default;
  ~RewriteSystemModel() override;

  [[nodiscard]] auto
  get_name() const -> std::string override {
    return "RewriteSystemModel";
  }

  auto parse(
      std::string &string) -> std::shared_ptr<RepresentationInterface> override {
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

  [[nodiscard]] auto get_representation_description() const noexcept -> std::string override {
    return nullptr;
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_REWRITESYSTEMMODEL_H
