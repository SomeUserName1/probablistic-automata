#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../models/ModelInterface.h"

class UserInterface {
public:
  enum Task {
    Unselected = 0,
    Reduction = 1,
    Equivalence = 2,
    Benchmark = 3,
    Conversion = 4,
    Exit = 5
  };
  enum IOMethod { Unse = 0, File = 1, Display = 2 };

  virtual ~UserInterface();

  [[nodiscard]] virtual auto select_task() const -> Task = 0;

  [[nodiscard]] virtual auto
  select_model(const std::vector<std::shared_ptr<ModelInterface>> &) const
      -> std::shared_ptr<ModelInterface> = 0;

  [[nodiscard]] virtual auto
  select_reduction_method(std::shared_ptr<ModelInterface> &selectedModel) const
      -> uint = 0;

  [[nodiscard]] virtual auto
  select_conversion_method(std::shared_ptr<ModelInterface> &) const
      -> std::shared_ptr<ConversionMethodInterface> = 0;

  [[nodiscard]] virtual auto select_io_method(bool) const -> IOMethod = 0;

  [[nodiscard]] virtual auto file_input() const -> std::string = 0;

  [[nodiscard]] virtual auto
  stdin_input(std::shared_ptr<ModelInterface> &) const -> std::string = 0;

  [[nodiscard]] virtual auto set_output_destination() const -> std::string = 0;

  static void display_file(const std::string &output, const std::string &file) {
    std::ofstream outfile(file);
    if (outfile.is_open()) {
      outfile << output;
      outfile.close();
      std::cout << "Output was written to " << file << std::endl;
    } else {
      throw std::bad_exception();
    }
  }

  virtual void display(const std::string & /*unused*/) const {
    throw std::logic_error(
        "Please use a concrete implementation of this interface!");
  }

  static auto read_file(const std::string &path) -> std::string {
    std::filesystem::path inputPath(path);
    inputPath = std::filesystem::canonical(inputPath);
    if (!inputPath.has_filename() || !std::filesystem::exists(inputPath)) {
      throw std::invalid_argument("Passed invalid input path " +
                                  inputPath.string());
    }
    std::cout << "Loading input from " << inputPath << std::endl;

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
    return contents;
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_USERINTERFACE_H
