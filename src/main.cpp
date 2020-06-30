#include <chrono>
#include <eigen3/Eigen/Core>
#include <filesystem>
#include <omp.h>
#include <tclap/CmdLine.h>

#include "models/differential_equations/DifferentialEquationModel.h"
#include "models/weighted_automata/WeightedAutomatonModel.h"
#include "models/rewrite_systems/RewriteSystemModel.h"
#include "ui/TextUserInterface.h"

#ifndef THREADS
#define THREADS 1
#endif

//static auto iequals(const std::string &str1, const std::string &str2) -> bool;

static auto iequals(const std::string &str1, const std::string &str2) -> bool {
  return str1.size() == str2.size() &&
         std::equal(str1.begin(), str1.end(), str2.begin(), [](auto a, auto b) {
           return std::tolower(a) == std::tolower(b);
         });
}

auto main(int argc, char *argv[]) -> int {
  omp_set_num_threads(THREADS);
  Eigen::initParallel();

  UserInterface::Task task = UserInterface::Task::Unselected;
  std::shared_ptr<ModelInterface> model = nullptr;
  uint reductionMethod = 0;
  UserInterface::IOMethod inputMethod = UserInterface::IOMethod::Unse;
  UserInterface::IOMethod outputMethod = UserInterface::IOMethod::Unse;
  std::string outputDestination;
  std::string input;
  std::string input1;
  std::shared_ptr<UserInterface> ui;

  try {
    TCLAP::CmdLine cmd("Stochastic dynamic system reducer", ' ', "0.1");

    TCLAP::SwitchArg tuiSwitch("T", "TextUserInterface",
                               "Use the text user interface as front end",
                               false);
    TCLAP::SwitchArg guiSwitch("G", "GraphicUserInterface",
                               "Use the graphic user interface as front end",
                               false);

    TCLAP::ValueArg<std::string> taskArg("t", "task", "Task to execute", false,
                                         "", "string");
    TCLAP::ValueArg<std::string> modelArg("m", "models", "models to use", false,
                                          "", "string");
    TCLAP::ValueArg<std::string> methodArg(
        "r", "reduction", "reduction method to apply", false, "", "string");
    TCLAP::ValueArg<std::string> inputArg("i", "input", "Path to input file",
                                          false, "", "string");
    TCLAP::ValueArg<std::string> input1Arg(
        "c", "input1", "Path to second input file fto compare first to", false,
        "", "string");
    TCLAP::ValueArg<std::string> outputArg("o", "output", "path to output file",
                                           false, "", "string");

    for (auto *arg :
         {&taskArg, &modelArg, &methodArg, &inputArg, &input1Arg, &outputArg}) {
      cmd.add(arg);
    }
    cmd.add(tuiSwitch);
    cmd.add(guiSwitch);
    cmd.parse(argc, argv);

    std::string taskStr = taskArg.getValue();
    std::string modelStr = modelArg.getValue();
    std::string methodStr = methodArg.getValue();
    std::string inputStr = inputArg.getValue();
    std::string input1Str = input1Arg.getValue();
    std::string outputStr = outputArg.getValue();
    bool tuiBool = tuiSwitch.getValue();
    bool guiBool = guiSwitch.getValue();

    if (!taskStr.empty() && !modelStr.empty() && !tuiBool && !guiBool &&
        !inputStr.empty() && !outputStr.empty()) {
      if (iequals(taskStr, "Reduction")) {
        task = UserInterface::Reduction;
      } else if (iequals(taskStr, "Equivalence")) {
        task = UserInterface::Equivalence;
      } else if (iequals(taskStr, "Benchmark")) {
        task = UserInterface::Benchmark;
      } else if (iequals(taskStr, "Conversion")) {
        task = UserInterface::Conversion;
      } else {
        std::cerr
            << "Specify either 'Reduction', 'Equivalence', 'Benchmark' or "
            << "'Conversion' as task, you  specified " + taskStr << std::endl;
        exit(-1);
      }
      if (iequals(modelStr, "WA") ||
          iequals(modelStr, "WeightedAutomatonModel") ||
          iequals(modelStr, "WeightedAutomaton")) {
        model = std::make_shared<WeightedAutomatonModel>();
      } else if (iequals(modelStr, "DE") ||
                 iequals(modelStr, "SystemOfEquationsModel") ||
                 iequals(modelStr, "DifferentialEquation")) {
        model = std::make_shared<DifferentialEquationModel>();
      } else if (iequals(modelStr, "RS") ||
                 iequals(modelStr, "RewritingSystemModel") ||
                 iequals(modelStr, "RewritingSystem")) {
        model = std::make_shared<DifferentialEquationModel>();
      } else {
        std::cerr
            << "Specify either 'WA', 'DE', 'RS', 'WeightedAutomatonModel', "
            << "'SystemOfEquationsModel', 'RewritingSystemMode', "
            << "'WeightedAutomaton', "
            << "'DifferentialEquation' or 'RewritingSystem'"
            << " as model, you specified " + modelStr << std::endl;
        exit(-1);
      }

      input = UserInterface::read_file(inputStr);

      std::filesystem::path outputPath(outputStr);
      if (outputPath.has_filename()) {
        outputDestination = outputStr;
      } else {
        std::cerr << "Please specify a path with a file name "
                  << "to write the results to!" << std::endl;
        exit(-1);
      }

      outputMethod = UserInterface::IOMethod::File;

      if (task == UserInterface::Reduction && !methodStr.empty()) {
        // currently only two methods are supported, one per models so ignore
        // what the user says and just use it Intentionally bad design,
        // sufficient for now
        reductionMethod = 0;
      }
      if (task == UserInterface::Equivalence && !input1Str.empty()) {
        input1 = UserInterface::read_file(input1Str);
      }

    } else {
      ui = std::make_shared<TextUserInterface>();
      if (guiBool) {
        std::cerr << "NotImplemented" << std::endl;
        exit(-1);
      }
      task = ui->select_task();
      switch (task) {
      case UserInterface::Exit: {
        exit(0);
      }
      case UserInterface::Reduction: {
        std::vector<std::shared_ptr<ModelInterface>> models = {
            std::make_shared<WeightedAutomatonModel>(),
            std::make_shared<DifferentialEquationModel>(),
            std::make_shared<RewriteSystemModel>()};
        model = ui->select_model(models);
        reductionMethod = ui->select_reduction_method(model);
        inputMethod = ui->select_io_method(true);
        outputMethod = ui->select_io_method(false);

        if (inputMethod == UserInterface::IOMethod::File) {
          input = ui->file_input();
        } else {
          input = ui->stdin_input(model);
        }
        if (outputMethod == UserInterface::IOMethod::File) {
          outputDestination = ui->set_output_destination();
        }
        break;
      }
      case UserInterface::Equivalence: {
        std::vector<std::shared_ptr<ModelInterface>> models = {
            std::make_shared<WeightedAutomatonModel>(),
            std::make_shared<DifferentialEquationModel>()};
        model = ui->select_model(models);
        inputMethod = ui->select_io_method(true);
        outputMethod = ui->select_io_method(false);

        if (inputMethod == UserInterface::IOMethod::File) {
          std::cout << "Automaton 1:" << std::endl;
          input = ui->file_input();
          std::cout << "Automaton 2:" << std::endl;
          input1 = ui->file_input();
        } else {
          std::cout << "Automaton 1:" << std::endl;
          input = ui->stdin_input(model);
          std::cout << "Automaton 2:" << std::endl;
          input1 = ui->stdin_input(model);
        }
        if (outputMethod == UserInterface::IOMethod::File) {
          outputDestination = ui->set_output_destination();
        }
        break;
      }
      case UserInterface::Benchmark:
      case UserInterface::Conversion:
      case UserInterface::Unselected:
        std::cerr << "not implemented" << std::endl;
        exit(-1);
      }
    }

    switch (task) {
    case UserInterface::Reduction: {
      auto representation = model->parse(input);
      auto start = std::chrono::high_resolution_clock::now();
      auto reduced_representation =
          model->get_reduction_methods()[reductionMethod]->reduce(
              representation);
      auto finish = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed = finish - start;
      std::cout << "Finished reduction in " << elapsed.count() << " s"
                << std::endl;
      auto summary =
          model->summarize_reduction(representation, reduced_representation);

      if (outputMethod == UserInterface::IOMethod::File) {
        ui->display_file(summary, outputDestination);
      } else if (outputMethod == UserInterface::IOMethod::Display) {
        ui->display(summary);
      }
      const bool result = representation->equivalent(reduced_representation);
      const std::string resStr = result ? "equivalent" : "not equivalent";
      std::cout << "Finished Equivalence check: " << resStr << std::endl;
      break;
    }
    case UserInterface::Equivalence: {
      auto representation0 = model->parse(input);
      auto representation1 = model->parse(input1);
      const bool result = representation0->equivalent(representation1);
      const std::string resStr = result ? "equivalent" : "not equivalent";
      std::cout << "Finished Equivalence check: " << resStr << std::endl;
      if (outputMethod == UserInterface::IOMethod::File) {
        ui->display_file(resStr, outputDestination);
      } else if (outputMethod == UserInterface::IOMethod::Display) {
        ui->display(resStr);
      }
      break;
    }
    case UserInterface::Benchmark:
    case UserInterface::Conversion:
    case UserInterface::Unselected: {
      throw NotImplementedException();
    }
    case UserInterface::Exit:
      std::cerr << "not implemented" << std::endl;
      exit(-1);
    }
  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId()
              << std::endl;
    exit(-1);
  }
}
