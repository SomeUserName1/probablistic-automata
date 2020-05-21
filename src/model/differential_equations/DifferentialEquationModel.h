#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H


#include <ModelInterface.h>

class DifferentialEquationModel : public ModelInterface {
    public:
        DifferentialEquationModel() = default;
        ~DifferentialEquationModel() override = default;

        std::string get_name() const override;

        std::shared_ptr<RepresentationInterface> validate_model_instance(const std::string &string) const override;

        std::string summarize_reduction(std::shared_ptr<RepresentationInterface> &ptr,
                                        std::shared_ptr<RepresentationInterface> &sharedPtr) const override;

        std::vector<std::shared_ptr<ReductionMethodInterface>> get_reduction_methods() const override;

        std::vector<std::shared_ptr<ConversionMethodInterface>> get_conversion_methods() const override;

        std::string get_representation_description() const override;
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_DIFFERENTIALEQUATIONMODEL_H
