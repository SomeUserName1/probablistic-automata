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
            Display = 2
        };
        /*
         * To allow polymorphism in the main, pure virtual is not possible. As the below functions are virtual, we
         * simply throw an exception, when an invalid call to the base class is made.
         */
        virtual ~UserInterface() = default;
        [[nodiscard]] virtual Task select_task() const = 0;
        [[nodiscard]] virtual std::shared_ptr<ModelInterface> select_model(const
        std::vector<std::shared_ptr<ModelInterface>>&) const = 0;
        [[nodiscard]] virtual std::shared_ptr<ReductionMethodInterface> select_reduction_method(
                std::shared_ptr<ModelInterface> &selectedModel) const = 0;
        [[nodiscard]] virtual std::shared_ptr<ConversionMethodInterface> select_conversion_method
        (std::shared_ptr<ModelInterface>& ) const = 0;
        [[nodiscard]] virtual IOMethod select_io_method(bool) const = 0;
        [[nodiscard]] virtual std::string file_input() const = 0;
        [[nodiscard]] virtual std::string stdin_input(std::shared_ptr<ModelInterface> &) const = 0;
        [[nodiscard]] virtual std::string set_output_destination() const = 0;
        static void display_file(const std::string& output, const std::string& file) {
            // TODO take care of creating the directories if non-existent
            std::ofstream outfile(file);
            outfile << output;
        }
        virtual void display(const std::string&) const {
            throw std::logic_error("Please use a concrete implementation of this interface!");
        }
        static std::string read_file(const std::string& path) {
            std::filesystem::path inputPath(path);
            inputPath = std::filesystem::canonical(inputPath);
            if (!inputPath.has_filename() || !std::filesystem::exists(inputPath)) {
                throw std::invalid_argument("Passed empty file as input with path " + inputPath.string());
            } else {
                std::cout << "Loading input from " << inputPath << std::endl;
            }

            std::ifstream in(inputPath);
            if (in.fail()) {
                throw std::invalid_argument("Failed to open input stream");
            }
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(static_cast<unsigned long>(in.tellg()));
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], static_cast<std::streamsize>(contents.size()));
            in.close();
            std::cout << contents << std::endl;
            return contents;
        }
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H
