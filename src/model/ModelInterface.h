#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H

#include <sstream>
#include <string>

#include "RepresentationInterface.h"
#include "ReductionMethodInterface.h"
#include "ConversionMethodInterface.h"

class ModelInterface {
    typedef RepresentationInterface (*ReductionMethodPointer)(RepresentationInterface);

    public:
        [[nodiscard]] virtual std::string get_name() const;
        [[nodiscard]] virtual RepresentationInterface validate_model_instance(const std::string&) const;
        [[nodiscard]] virtual std::stringstream summarize_reduction(RepresentationInterface, RepresentationInterface)
            const;
        // ReductionMethodPointer reductionMethods[1];
        // ConversionMethodInterface* conversionMethods[1];
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
