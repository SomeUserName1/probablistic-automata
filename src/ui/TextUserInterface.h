#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_TEXTUSERINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_TEXTUSERINTERFACE_H

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "UserInterface.h"

class TextUserInterface : public UserInterface {
public:
    [[nodiscard]] Task select_task() const override;
    [[nodiscard]] const ModelInterface& select_model(const std::vector<ModelInterface>&) const override;
    [[nodiscard]] const ReductionMethodInterface& select_reduction_method(const ModelInterface&) const override;
    [[nodiscard]] const ConversionMethodInterface& select_conversion_method(const ModelInterface&) const override;
    [[nodiscard]] IOMethod select_io_method() const override;
    [[nodiscard]] std::string file_input() const override;
    [[nodiscard]] std::string stdin_input() const override;
    [[nodiscard]] std::string set_output_destination() const override;
    static void display_file(const std::string&, const std::string&) const override;
    static void display_stdout(const std::string&) const override;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_TEXTUSERINTERFACE_H
