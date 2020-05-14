#include "differential_equations/DifferentialEquationModel.h"
#include "weighted_automata/WeightedAutomatonModel.h"
#include "ui/TextUserInterface.cpp"

int main(int argc, char* argv[]) {
    UserInterface::Task task;
    ModelInterface model;
    ReductionMethodInterface reductionMethod;
    UserInterface::IOMethod inputMethod;
    UserInterface::IOMethod outputMethod;
    std::string outputDestination;
    std::string input;

    if (argc == 0) {
        auto ui = std::make_unique<TextUserInterface>(TextUserInterface());
        task = ui->select_task();

        switch (task) {
            case UserInterface::Reduction: {
                std::vector<ModelInterface> models = {WeightedAutomatonModel(), DifferentialEquationModel()};
                model = ui->select_model(models);
                reductionMethod = ui->select_reduction_method(model);
                inputMethod = ui->select_io_method();
                outputMethod = ui->select_io_method();

                if (inputMethod == UserInterface::IOMethod::File) {
                    input = ui->file_input();
                } else {
                    input = ui->stdin_input();
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
    } else if (argc == 1) {
    } else {

    }

    switch (task) {
        case UserInterface::Reduction: {
            const auto representation = model.validate_model_instance(input);
            const auto reduced_representation = reductionMethod.reduce(representation);
            const auto summary = model.summarize_reduction(representation, reduced_representation);

            if (outputMethod == UserInterface::IOMethod::File) {
                TextUserInterface::display_file(summary, outputDestination);
            } else if (outputMethod == UserInterface::IOMethod::Stdin) {
                TextUserInterface::display_stdout(summary);
            }
            break;
        }
        case UserInterface::Benchmark:
        case UserInterface::Conversion:
        case UserInterface::Unselected:
            throw NotImplementedException();
    }

}




