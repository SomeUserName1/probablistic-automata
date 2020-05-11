#include <ios>
#include <string>

#include "ui/TextUserInterface.cpp"

int main(int argc, char* argv[]) {
    auto* ui = new TextUserInterface();
    UserInterface::Task task = ui->select_task();

    switch (task) {
        case UserInterface::Reduction: {
            ModelInterface model = ui->select_model();
            ReductionMethodInterface reductionMethod = ui->select_reduction_method(model);
            UserInterface::IOMethod inputMethod = ui->select_io_method();
            UserInterface::IOMethod outputMethod = ui->select_io_method();
            std::stringstream input;
            if (inputMethod == TextUserInterface::IOMethod::File) {
                input = ui->file_input();
            } else {
                input = ui->stdin_input();
            }
            break;
        }
        case UserInterface::Benchmark: {
            break;
        }
        case UserInterface::Conversion: {
            break;
        }
        case UserInterface::Unselected:
            break;
    }
}

