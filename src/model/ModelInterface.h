#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H

#include <sstream>
#include <string>
#include <vector>

#include "RepresentationInterface.h"
#include "ReductionMethodInterface.h"
#include "ConversionMethodInterface.h"

class ModelInterface {

    public:
        [[nodiscard]] virtual std::string get_name() const;
        [[nodiscard]] virtual RepresentationInterface validate_model_instance(const std::string&) const;
        [[nodiscard]] virtual std::string summarize_reduction(RepresentationInterface, RepresentationInterface)
            const;
        virtual std::vector<ReductionMethodInterface*> get_reduction_methods() const;
        virtual std::vector<ConversionMethodInterface*> get_conversion_methods() const;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
