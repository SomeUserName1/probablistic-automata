#include <tclap/CmdLine.h>

#include "differential_equations/DifferentialEquationModel.h"
#include "weighted_automata/WeightedAutomatonModel.h"
#include "ui/TextUserInterface.cpp"
#include "NotImplementedException.h"

// FIXME when everything is implemented: smart pointers and references wrt functions/passing & returning

bool iequals(const std::string&, const std::string&);

bool iequals(const std::string& str1, const std::string& str2) {
    return str1.size() == str2.size()
        && std::equal(str1.begin(), str1.end(), str2.begin(),
                [](auto a, auto b){
                    return std::tolower(a)==std::tolower(b);
                }
            );
}

int main(int argc, char* argv[]) {
    UserInterface::Task task;
    std::shared_ptr<ModelInterface> model;
    std::shared_ptr<ReductionMethodInterface> reductionMethod;
    UserInterface::IOMethod inputMethod;
    UserInterface::IOMethod outputMethod = UserInterface::IOMethod::Unse;
    std::string outputDestination = "init";
    std::string input = "init";
    std::shared_ptr<UserInterface> ui;

    try {
        TCLAP::CmdLine cmd("Stochastic dynamic system reducer", ' ', "0.1");

        TCLAP::SwitchArg tuiSwitch("tui","TextUserInterface",
                "Use the text user interface as front end", false);
        TCLAP::SwitchArg guiSwitch("gui","GraphicUserInterface",
                "Use the graphic user interface as front end", false);

        TCLAP::ValueArg<std::string> taskArg("t","task","Task to execute",
                false,"","string");
        TCLAP::ValueArg<std::string> modelArg("m","model","model to use",
                false,"","string");
        TCLAP::ValueArg<std::string> methodArg("r","reduction","reduction method to apply",
                false,"","string");
        TCLAP::ValueArg<std::string> inputArg("i","input","Path to input file",
                false,"","string");
        TCLAP::ValueArg<std::string> outputArg("o","output","path to output file",
                false,"","string");

        for (auto arg : {&taskArg, &modelArg, &methodArg, &inputArg, &outputArg} ) {
            cmd.add(arg);
        }
        cmd.xorAdd(tuiSwitch, guiSwitch);
        cmd.parse(argc, argv);

        std::string taskStr = taskArg.getValue();
        std::string modelStr = modelArg.getValue();
        std::string methodStr = methodArg.getValue();
        std::string inputStr = inputArg.getValue();
        std::string outputStr = outputArg.getValue();

        if (!taskStr.empty() && !modelStr.empty() && !methodStr.empty() && !inputStr.empty() && !outputStr.empty()
            && !tuiSwitch && !guiSwitch) {
            if (iequals(taskStr,"Reduction")) {
                task = UserInterface::Reduction;
            } else if (iequals(taskStr,"Benchmark")) {
                task = UserInterface::Benchmark;
            } else if (iequals(taskStr, "Conversion")) {
                task = UserInterface::Conversion;
            } else {
                throw std::invalid_argument("Specify either 'Reduction', 'Benchmark' or 'Conversion' as task");
            }
            if (iequals(taskStr,"WA") || iequals(taskStr,"WeightedAutomatonModel")
                || iequals(taskStr,"WeightedAutomaton")) {
                model = std::make_shared<WeightedAutomatonModel>();
            } else if (iequals(taskStr,"DE") || iequals(taskStr,"DifferentialEquationModel")
                || iequals(taskStr,"Differential Equation")) {
                model = std::make_shared<DifferentialEquationModel>();
            } else {
                throw std::invalid_argument("Specify either 'WA', 'DE', 'WeightedAutomatonModel', "
                                            "'DifferentialEquationModel', 'WeightedAutomaton' or 'DifferentialEquation'"
                                            " as model'");
            }
            // currently only two methods are supported, one per model so ignore what the user says and just use it
            // Intentionally bad design, FIXME
            reductionMethod = model->get_reduction_methods()[0];
            std::filesystem::path inputPath(inputStr);
            if (inputPath.has_filename() && std::filesystem::exists(inputPath)) {
                input = UserInterface::read_file(inputStr);
            }
            std::filesystem::path outputPath(outputStr);
            if (inputPath.has_filename()) {
                outputDestination = outputStr;
            }
        } else {
            if (argc == 0 || tuiSwitch) {
                ui = std::make_shared<TextUserInterface>(TextUserInterface());
            } else if (guiSwitch) {
                throw NotImplementedException();
            }
            task = ui->select_task();
            switch (task) {
                case UserInterface::Reduction: {
                    std::vector<std::shared_ptr<ModelInterface>> models = {std::make_shared<WeightedAutomatonModel>(),
                            std::make_shared<DifferentialEquationModel>()};
                    model = ui->select_model(models);
                    reductionMethod = ui->select_reduction_method(model);
                    inputMethod = ui->select_io_method();
                    outputMethod = ui->select_io_method();

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
                case UserInterface::Benchmark:
                case UserInterface::Conversion:
                case UserInterface::Unselected:
                    throw NotImplementedException();
            }
        }

        switch (task) {
            case UserInterface::Reduction: {
                auto representation = model->validate_model_instance(input);
                auto reduced_representation = reductionMethod->reduce(representation);
                auto summary = model->summarize_reduction(representation, reduced_representation);

                if (outputMethod == UserInterface::IOMethod::File) {
                    ui->display_file(summary, outputDestination);
                } else if (outputMethod == UserInterface::IOMethod::Display) {
                    ui->display(summary);
                }
                break;
            }
            case UserInterface::Benchmark:
            case UserInterface::Conversion:
            case UserInterface::Unselected:
                throw NotImplementedException();
        }
    } catch (TCLAP::ArgException &e)  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}
