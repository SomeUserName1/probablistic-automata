#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H

#include <string>
#include <ios>

#include "ModelInterface.h"


class UserInterface {
    public:
        enum Task {
            Unselected = 0,
            Reduction = 1,
            Benchmark = 2,
            Conversion = 3
        };
        enum Model {
            Uns = 0,
            WA = 1,
            DE = 2
        };
        enum IOMethod {
            Unse = 0,
            File = 1,
            Stdin = 2
        };
        [[nodiscard]] virtual Task select_task() const = 0;
        [[nodiscard]] virtual const ModelInterface& select_model(const std::vector<ModelInterface>&) const = 0;
        [[nodiscard]] virtual const ModelInterface& select_model() const = 0;
        [[nodiscard]] virtual const ReductionMethodInterface& select_reduction_method(const ModelInterface&) const = 0;
        [[nodiscard]] virtual const ConversionMethodInterface& select_conversion_method(const ModelInterface&,
                const ModelInterface&) const = 0;
        [[nodiscard]] virtual IOMethod select_io_method() const = 0;
        [[nodiscard]] virtual std::stringstream file_input() const = 0;
        [[nodiscard]] virtual std::stringstream stdin_input() const = 0;
        [[nodiscard]] virtual const std::string& set_output_destination() const = 0;
        virtual void display_file(const std::stringstream&) const = 0;
        virtual void display_stdout(const std::stringstream&) const = 0;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H
