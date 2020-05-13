#include <differential_equations/DifferentialEquationModel.h>
#include <weighted_automata/WeightedAutomatonModel.h>
#include "ui/TextUserInterface.cpp"

int main(int argc, char* argv[]) {
    auto* ui = new TextUserInterface();
    auto task = ui->select_task();

    switch (task) {
        case UserInterface::Reduction: {
            std::vector<ModelInterface> models = {WeightedAutomatonModel(), DifferentialEquationModel()};
            const auto& model = ui->select_model(models);
            const auto& reductionMethod = ui->select_reduction_method(model);
            const auto inputMethod = ui->select_io_method();
            std::string input;
            if (inputMethod == TextUserInterface::IOMethod::File) {
                input = ui->file_input();
            } else {
                input = ui->stdin_input();
            }
            const auto representation = model.validate_model_instance(input);
            const auto reduced_representation = reductionMethod.reduce(representation);
            const auto outputMethod = ui->select_io_method();
            const auto summary = model.summarize_reduction(representation, reduced_representation);
            if (outputMethod == TextUserInterface::IOMethod::File) {
                const auto& outputDestination = ui->set_output_destination();
                ui->display_file(summary, outputDestination);
            }
            break;
        }
        case UserInterface::Benchmark:
        case UserInterface::Conversion:
        case UserInterface::Unselected:
            throw NotImplementedException();
    }
}




