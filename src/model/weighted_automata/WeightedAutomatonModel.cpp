#include "WeightedAutomatonModel.h"

#include <utility>
#include "KieferSchuetzenbergerReduction.h"

WeightedAutomatonModel::WeightedAutomatonModel() {
    this->reductionMethods = {std::make_shared<KieferSchuetzenbergerReduction>()};
    this->conversionMethods = {};
}

void WeightedAutomatonModel::set_instance(std::shared_ptr<RepresentationInterface>& modelInstance) {
    this->instance = std::move(modelInstance);
}

std::string WeightedAutomatonModel::get_name() const {
    return "Weighted Automaton Model";
}

RepresentationInterface WeightedAutomatonModel::validate_model_instance(const std::string &string) const {
    return ModelInterface::validate_model_instance(string);
}

std::string WeightedAutomatonModel::summarize_reduction(RepresentationInterface anInterface,
                                                        RepresentationInterface representationInterface) const {
    return ModelInterface::summarize_reduction(anInterface, representationInterface);
}

bool WeightedAutomatonModel::is_wa() const {
    return true;
}

bool WeightedAutomatonModel::is_de() const {
    return false;
}

std::vector<std::shared_ptr<ReductionMethodInterface>> WeightedAutomatonModel::get_reduction_methods() const {
    return this->reductionMethods;
}

std::vector<std::shared_ptr<ConversionMethodInterface>> WeightedAutomatonModel::get_conversion_methods() const {
    return this->conversionMethods;
}
