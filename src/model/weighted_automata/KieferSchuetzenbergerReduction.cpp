#include "KieferSchuetzenbergerReduction.h"

std::string KieferSchuetzenbergerReduction::get_name() const {
    return "Random Basis Schützenberger Reduction";
}

std::shared_ptr<RepresentationInterface> KieferSchuetzenbergerReduction::reduce
(std::shared_ptr<RepresentationInterface> &anInterface)
const {
    return ReductionMethodInterface::reduce(anInterface);
}
