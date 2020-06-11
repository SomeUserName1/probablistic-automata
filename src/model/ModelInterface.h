#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ConversionMethodInterface.h"
#include "ReductionMethodInterface.h"
#include "RepresentationInterface.h"

class ModelInterface {

public:
  virtual ~ModelInterface();

  [[nodiscard]] virtual auto get_name() const -> std::string = 0;

  [[nodiscard]] virtual auto
  validate_model_instance(std::string &) -> std::shared_ptr<RepresentationInterface> = 0;

  [[nodiscard]] virtual auto
  summarize_reduction(std::shared_ptr<RepresentationInterface> &,
                      std::shared_ptr<RepresentationInterface> &) const -> std::string = 0;

  [[nodiscard]] virtual auto
  get_reduction_methods() const -> std::vector<std::shared_ptr<ReductionMethodInterface>> = 0;

  [[nodiscard]] virtual auto
  get_conversion_methods() const -> std::vector<std::shared_ptr<ConversionMethodInterface>> = 0;

  [[nodiscard]] virtual auto get_representation_description() const noexcept -> std::string = 0;
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
