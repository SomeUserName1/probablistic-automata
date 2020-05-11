#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_TEXTUSERINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_TEXTUSERINTERFACE_H

#include <iostream>

#include "UserInterface.h"

class TextUserInterface : public UserInterface {
public:
    [[nodiscard]] Task select_task() const override;
    [[nodiscard]] const ModelInterface& select_model(const std::vector<ModelInterface>&) const override;
    [[nodiscard]] const ModelInterface& select_model() const override;
    [[nodiscard]] const ReductionMethodInterface& select_reduction_method(const ModelInterface&) const override;
    [[nodiscard]] const ConversionMethodInterface& select_conversion_method(const ModelInterface&,
            const ModelInterface&) const override;
    [[nodiscard]] IOMethod select_io_method() const override;
    [[nodiscard]] std::stringstream file_input() const override;
    [[nodiscard]] std::stringstream stdin_input() const override;
    [[nodiscard]] const std::string& set_output_destination() const override;
    void display_file(const std::stringstream&) const override;
    void display_stdout(const std::stringstream&) const override;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_TEXTUSERINTERFACE_H
