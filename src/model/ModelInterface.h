#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H

#include <sstream>
#include <string>
#include <vector>
#include <memory>


#include "RepresentationInterface.h"
#include "ReductionMethodInterface.h"
#include "ConversionMethodInterface.h"

class ModelInterface {

public:
    virtual ~ModelInterface();

    [[nodiscard]] virtual std::string get_name() const = 0;

    [[nodiscard]] virtual std::shared_ptr<RepresentationInterface> validate_model_instance(std::string &)
    const = 0;

    [[nodiscard]] virtual std::string summarize_reduction(std::shared_ptr<RepresentationInterface> &,
                                                          std::shared_ptr<RepresentationInterface> &)
    const = 0;

    [[nodiscard]] virtual std::vector<std::shared_ptr<ReductionMethodInterface>> get_reduction_methods() const = 0;

    [[nodiscard]] virtual std::vector<std::shared_ptr<ConversionMethodInterface>> get_conversion_methods() const
    = 0;

    virtual std::string get_representation_description() const noexcept = 0;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
