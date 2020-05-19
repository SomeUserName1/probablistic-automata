//
// Created by someusername on 19.05.20.
//

#include "KieferSchuetzenbergerReduction.h"

std::string KieferSchuetzenbergerReduction::get_name() const {
    return "Random Basis Schützenberger Reduction";
}

RepresentationInterface KieferSchuetzenbergerReduction::reduce(RepresentationInterface anInterface) const {
    return ReductionMethodInterface::reduce(anInterface);
}
