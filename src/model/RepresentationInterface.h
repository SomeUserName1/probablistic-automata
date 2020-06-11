#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H

#include <memory>
#include "../NotImplementedException.h"

// Used to avoid a cyclic dependency: ModelInterface to ReductionMethodInterface to ModelInterface
class RepresentationInterface {
public:
    virtual ~RepresentationInterface();

    [[nodiscard]] virtual auto pretty_print() const -> std::string = 0;

    virtual auto operator==(const std::shared_ptr<RepresentationInterface>&) const -> bool = 0;

    virtual auto operator!=(const std::shared_ptr<RepresentationInterface>&) const -> bool = 0;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELREPRESENTATIONINTERFACE_H
