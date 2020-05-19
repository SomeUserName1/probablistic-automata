#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H


#include <ModelInterface.h>

class WeightedAutomatonModel : public ModelInterface {
private:
    std::shared_ptr<RepresentationInterface> instance;
    std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
    std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;
public:
    WeightedAutomatonModel();
    [[nodiscard]] std::string get_name() const override;

    [[nodiscard]] RepresentationInterface validate_model_instance(const std::string &string) const override;

    [[nodiscard]] std::string summarize_reduction(RepresentationInterface anInterface,
                                    RepresentationInterface representationInterface) const override;

    [[nodiscard]] bool is_wa() const override;

    [[nodiscard]] bool is_de() const override;

    [[nodiscard]] std::vector<std::shared_ptr<ReductionMethodInterface>> get_reduction_methods() const override;

    [[nodiscard]] std::vector<std::shared_ptr<ConversionMethodInterface>> get_conversion_methods() const override;

    void set_instance(std::shared_ptr<RepresentationInterface>&) override;

};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
