#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H

#include <string>
#include <ios>
#include <stdexcept>

#include "ModelInterface.h"


class UserInterface {
    public:
        enum Task {
            Unselected = 0,
            Reduction = 1,
            Benchmark = 2,
            Conversion = 3
        };
        enum IOMethod {
            Unse = 0,
            File = 1,
            Stdin = 2
        };
        /*
         * To allow polymorphism in the main, pure virtual is not possible. As the below functions are virtual, we
         * simply throw an exception, when an invalid call to the base class is made.
         */
        [[nodiscard]] virtual Task select_task() const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        }
        [[nodiscard]] virtual const ModelInterface& select_model(const std::vector<ModelInterface>&) const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
        [[nodiscard]] virtual const ReductionMethodInterface& select_reduction_method(const ModelInterface&) const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
        [[nodiscard]] virtual const ConversionMethodInterface& select_conversion_method(const ModelInterface&) const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
        [[nodiscard]] virtual IOMethod select_io_method() const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
        [[nodiscard]] virtual std::string file_input() const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
        [[nodiscard]] virtual std::string stdin_input() const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
        [[nodiscard]] virtual std::string set_output_destination() const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
        virtual void display_file(const std::string&, const std::string&) const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
        virtual void display_stdout(const std::string&) const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        };
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H
