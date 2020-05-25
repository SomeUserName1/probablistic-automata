#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H

#include <utility>

#include "ModelInterface.h"
#include "KieferSchuetzenbergerReduction.h"
#include "WeightedAutomatonInstance.h"
#include "../../NotImplementedException.h"

class WeightedAutomatonModel : public ModelInterface {
private:
    std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
    std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;
public:
    WeightedAutomatonModel() : reductionMethods({std::make_shared<KieferSchuetzenbergerReduction>()}),
        conversionMethods({}) {};

    [[nodiscard]] std::string get_name() const override;

    [[nodiscard]] std::shared_ptr<RepresentationInterface> validate_model_instance(const std::string &string) const
    override;

    [[nodiscard]] std::string summarize_reduction(std::shared_ptr<RepresentationInterface>&,
                                                  std::shared_ptr<RepresentationInterface>&)
                                                  const override;

    [[nodiscard]] std::vector<std::shared_ptr<ReductionMethodInterface>> get_reduction_methods() const override;

    [[nodiscard]] std::vector<std::shared_ptr<ConversionMethodInterface>> get_conversion_methods() const override;


    std::string get_representation_description() const override {
        return "You need to specify 5 variables: \n "
               "The number of states by: states=\n"
               "The number of characters in the input alphabet: characters=\n"
               "The initial vector: alpha="
               "The transition matrices (one per input character): muX=\n"
               "The final vector: eta=\n\n"
               "Example:\n"
               "states=4;\n"
               "characters=2;\n"
               "alpha=(1,0,0,0);\n"
               "mu1=((0,1,1,0),(0,0,0,0),(0,0,0,0),(0,0,0,0));\n"
               "mu2=((0,0,0,0),(0,0,0,1),(0,0,0,1),(0,0,0,0));\n"
               "eta=(0,0,0,1);\n";
    }

    std::tuple<std::string, size_t> get_next_line(const std::string &str, size_t prev) const;

    std::tuple<float, std::string> extract_one_digit(const std::string &vector) const;
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
