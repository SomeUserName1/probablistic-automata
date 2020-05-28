#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H

#include "../NotImplementedException.h"

// Used to avoid a cyclic dependency: ModelInterface to ReductionMethodInterface to ModelInterface
class RepresentationInterface {
    public:
        virtual ~RepresentationInterface() = default;
        virtual const std::string pretty_print() const = 0;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H
