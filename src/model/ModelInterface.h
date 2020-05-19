#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H

#include <sstream>
#include <string>
#include <vector>
#include <memory>


#include "RepresentationInterface.h"
#include "ReductionMethodInterface.h"
#include "ConversionMethodInterface.h"

class ModelInterface {

    public:
        [[nodiscard]] virtual std::string get_name() const {
            throw NotImplementedException();
        }
        [[nodiscard]] virtual RepresentationInterface validate_model_instance(const std::string&) const {
            throw NotImplementedException();
        }
        [[nodiscard]] virtual std::string summarize_reduction(RepresentationInterface, RepresentationInterface)
            const {
            throw NotImplementedException();
        }
        [[nodiscard]] virtual bool is_wa() const {
            throw NotImplementedException();
        }
        [[nodiscard]] virtual bool is_de() const {
            throw NotImplementedException();
        }
        [[nodiscard]] virtual std::vector<std::shared_ptr<ReductionMethodInterface>> get_reduction_methods() const {
            throw NotImplementedException();
        }
        [[nodiscard]] virtual std::vector<std::shared_ptr<ConversionMethodInterface>> get_conversion_methods() const {
            throw NotImplementedException();
        }
        virtual void set_instance(std::shared_ptr<RepresentationInterface>&) {
            throw NotImplementedException();
        }
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELINTERFACE_H
