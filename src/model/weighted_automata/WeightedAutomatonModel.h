#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H

#include <utility>

#include "../ModelInterface.h"
#include "KieferSchuetzenbergerReduction.h"
#include "WeightedAutomatonInstance.h"

template<typename T>
concept Arithmetic = std::is_arithmetic<T>::value;

typedef unsigned int uint;

class WeightedAutomatonModel : public ModelInterface {
private:
    std::vector<std::shared_ptr<ReductionMethodInterface>> reductionMethods;
    std::vector<std::shared_ptr<ConversionMethodInterface>> conversionMethods;
public:
    WeightedAutomatonModel() : reductionMethods({std::make_shared<KieferSchuetzenbergerReduction>()}),
                               conversionMethods({}) {}

    [[nodiscard]] std::string get_name() const override;

    std::shared_ptr<RepresentationInterface> validate_model_instance(std::string &string) const override;

    static std::shared_ptr<RepresentationInterface> validate_model_instance_dense(std::string &string);

    static std::shared_ptr<RepresentationInterface> validate_model_instance_sparse(std::string &string);

    [[nodiscard]] std::string summarize_reduction(std::shared_ptr<RepresentationInterface> &,
                                                  std::shared_ptr<RepresentationInterface> &)
    const override;

    [[nodiscard]] std::vector<std::shared_ptr<ReductionMethodInterface>> get_reduction_methods() const override;

    [[nodiscard]] std::vector<std::shared_ptr<ConversionMethodInterface>> get_conversion_methods() const override;


    std::string get_representation_description() const noexcept override {
        return "You need to specify one flag and 5 variables: \n "
               "The input type: input"
               "The number of states by: states\n"
               "The number of characters in the input alphabet: characters=\n"
               "The initial vector: alpha \n"
               "The transition matrices (one per input character): muX\n"
               "The final vector: eta\n\n\n"
               "Example for the dense type:\n"
               "input=dense\n"
               "states=4;\n"
               "characters=2;\n"
               "alpha=(1,0,0,0);\n"
               "mu1=((0,1,1,0),(0,0,0,0),(0,0,0,0),(0,0,0,0));\n"
               "mu2=((0,0,0,0),(0,0,0,1),(0,0,0,1),(0,0,0,0));\n"
               "eta=(0,0,0,1);\n\n\n"
               "Example for the sparse type:\n"
               "input=sparse\n"
               "states=4;\n"
               "characters=2;\n"
               "alpha: 0 1\n"
               "mu\n"
               "0 0 1 1;\n"
               "0 0 2 1;\n"
               "1 1 3 1;\n"
               "1 2 3 1;\n"
               "eta: 3 1;\n";
    }

    inline static std::string get_next_line(std::string &str, bool trimSpaces)
    noexcept {
        std::string line;
        std::size_t pos = str.find(';');
        line = str.substr(0, pos + 1);
        str.erase(0, pos + 1);
        if (trimSpaces) {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        } else {
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        }
        return line;
    }


    template<Arithmetic T>
    static inline T extract_one_digit(std::string &vector) {
        bool prevDigit = false;
        size_t firstDigit = 0;
        for (size_t i = 0; i < vector.size(); i++) {
            if (std::isdigit(vector[i]) && !prevDigit) {
                prevDigit = true;
                firstDigit = i;
            } else if (!std::isdigit(vector[i]) && prevDigit) {
                T result = static_cast<T>(std::stod(vector.substr(firstDigit, i)));
                vector.erase(0, i);
                return result;
            }
        }
        throw std::invalid_argument("No numbers found in the input!");
    }
};


#endif //STOCHASTIC_SYSTEM_MINIMIZATION_WEIGHTEDAUTOMATONMODEL_H
