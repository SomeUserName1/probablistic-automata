//
// Created by someusername on 19.05.20.
//

#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H


#include <ReductionMethodInterface.h>
#include <string>

class KieferSchuetzenbergerReduction : public ReductionMethodInterface {
public:
    ~KieferSchuetzenbergerReduction() override = default;
    [[nodiscard]] std::string get_name() const override;

    [[nodiscard]] RepresentationInterface reduce(RepresentationInterface anInterface) const override;
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
