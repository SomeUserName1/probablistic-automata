#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H

#include "RepresentationInterface.h"
#include <memory>

class ReductionMethodInterface {
public:
    virtual ~ReductionMethodInterface();

    virtual std::string get_name() const = 0;

    virtual std::shared_ptr<RepresentationInterface> reduce(std::shared_ptr<RepresentationInterface> &) const = 0;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_REDUCTIONMETHODINTERFACE_H
