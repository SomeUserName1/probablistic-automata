#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H


#include "../ModelInterface.h"
#include "../RepresentationInterface.h"

class DifferentialEquationModel : public ModelInterface {
public:
    DifferentialEquationModel() = default;

    ~DifferentialEquationModel() override;

    std::string get_name() const override {
        return "Differential Equation Model";
    }

    std::shared_ptr<RepresentationInterface> validate_model_instance(std::string &string) override {
        auto a = string;
        throw NotImplementedException();
    }

    std::string summarize_reduction(std::shared_ptr<RepresentationInterface> &ptr,
                                    std::shared_ptr<RepresentationInterface> &sharedPtr) const override {
        auto a = ptr;
        auto b = sharedPtr;
        throw NotImplementedException();
    }

    std::vector<std::shared_ptr<ReductionMethodInterface>> get_reduction_methods() const override {
        throw NotImplementedException();
    }

    std::vector<std::shared_ptr<ConversionMethodInterface>> get_conversion_methods() const override {
        throw NotImplementedException();
    }

    std::string get_representation_description() const noexcept override {
        return "TODO";
    }
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H
