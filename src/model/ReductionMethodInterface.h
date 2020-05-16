#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H

#include "RepresentationInterface.h"

class ReductionMethodInterface {
    public:
        virtual std::string get_name() const;
        virtual RepresentationInterface reduce(RepresentationInterface) const;
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H