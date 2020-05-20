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
    ~TextUserInterface() override = default;
    [[nodiscard]] Task select_task() const override;
    [[nodiscard]] std::shared_ptr<ModelInterface> select_model(const std::vector<std::shared_ptr<ModelInterface>>&)
        const override;
    [[nodiscard]] std::shared_ptr<ReductionMethodInterface> select_reduction_method(std::shared_ptr<ModelInterface>&)
        const override;
    [[nodiscard]] std::shared_ptr<ConversionMethodInterface> select_conversion_method(std::shared_ptr<ModelInterface>& )
        const override;
    [[nodiscard]] IOMethod select_io_method() const override;
    [[nodiscard]] std::string file_input() const override;
    [[nodiscard]] std::string stdin_input(std::shared_ptr<ModelInterface> &) const override;
    [[nodiscard]] std::string set_output_destination() const override;
    void display(const std::string&) const override;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_TEXTUSERINTERFACE_H
