#include "DifferentialEquationModel.h"
#include "../../NotImplementedException.h"

std::shared_ptr<RepresentationInterface>
DifferentialEquationModel::validate_model_instance(const std::string &) const {
    throw NotImplementedException();
}

std::string DifferentialEquationModel::summarize_reduction(std::shared_ptr<RepresentationInterface> &,
                                                           std::shared_ptr<RepresentationInterface> &) const {
    throw NotImplementedException();
}

std::string DifferentialEquationModel::get_name() const {
    return "Differential Equation Model";
}

std::vector<std::shared_ptr<ReductionMethodInterface>> DifferentialEquationModel::get_reduction_methods() const {
    throw NotImplementedException();
}

std::vector<std::shared_ptr<ConversionMethodInterface>> DifferentialEquationModel::get_conversion_methods() const {
    throw NotImplementedException();
}

std::string DifferentialEquationModel::get_representation_description() const {
    throw NotImplementedException();
}
