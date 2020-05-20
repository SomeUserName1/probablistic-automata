#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H

#include <utility>

#include <ModelInterface.h>
#include "KieferSchuetzenbergerReduction.h"
#include "WeightedAutomatonInstance.h"

class WeightedAutomatonModel : public ModelInterface {
private:
    std::shared_ptr<RepresentationInterface> instance;
    std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
    std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;
public:
    WeightedAutomatonModel() : instance(std::make_shared<WeightedAutomatonInstance>()),
        reductionMethods({std::make_shared<KieferSchuetzenbergerReduction>()}), conversionMethods({}) {};

    [[nodiscard]] std::string get_name() const override;

    [[nodiscard]] std::shared_ptr<RepresentationInterface> validate_model_instance(const std::string &string) const
    override;

    [[nodiscard]] std::string summarize_reduction(std::shared_ptr<RepresentationInterface>&,
                                                  std::shared_ptr<RepresentationInterface>&)
                                                  const override;

    [[nodiscard]] std::vector<std::shared_ptr<ReductionMethodInterface>> get_reduction_methods() const override;

    [[nodiscard]] std::vector<std::shared_ptr<ConversionMethodInterface>> get_conversion_methods() const override;

    void set_instance(std::shared_ptr<RepresentationInterface>&) override;

    std::tuple<float, std::string> extract_one_digit(std::string) const;
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
