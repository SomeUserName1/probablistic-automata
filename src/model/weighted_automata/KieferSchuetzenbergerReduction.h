//
// Created by someusername on 19.05.20.
//

#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H

#include <string>
#include <memory>

#include <ReductionMethodInterface.h>

class KieferSchuetzenbergerReduction : public ReductionMethodInterface {
public:
    ~KieferSchuetzenbergerReduction() override = default;
    [[nodiscard]] std::string get_name() const override;

    [[nodiscard]] std::shared_ptr<RepresentationInterface> reduce(std::shared_ptr<RepresentationInterface>&)
    const override;
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
