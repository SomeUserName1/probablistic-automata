#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H

#include "RepresentationInterface.h"

class ReductionMethodInterface {
    public:
        virtual ~ReductionMethodInterface() = default;;
        [[nodiscard]] virtual std::string get_name() const {
            throw NotImplementedException();
        }

    [[nodiscard]] virtual RepresentationInterface reduce(RepresentationInterface) const {
            throw NotImplementedException();
        }
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H
