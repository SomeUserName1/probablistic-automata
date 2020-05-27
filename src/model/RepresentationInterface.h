#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H

#include "../NotImplementedException.h"

class RepresentationInterface {
    public:
        virtual ~RepresentationInterface() = default;
        virtual const std::string pretty_print() const = 0;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H
