#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include "UserInterface.h"
#include "TextUserInterface.h"

UserInterface::Task TextUserInterface::select_task() const {
    Task task = Unselected;
    std::cout << "Hello," << std::endl
              << "this is a program to automatically minimize models of stochastic dynamical systems like automata"
              << "or differential equations." << std::endl
              << "What do you want to do?" << std::endl
              << "\t\t 1. Reduce an example" << std::endl
              << "\t\t 2. Perform a benchmark" << std::endl
              << "\t\t 3. Convert an example from one model to another" << std::endl;

    do {
        std::string input;
        std::cin >> input;
        if (input.empty() || input.length() > 1 ||
            (!(input == "1") && !(input == "2") && !(input == "3"))) {
            std::cout << "Please enter 1, 2 or 3!";
            continue;
        } else {
            task = Task(std::stoi(input));
        }
    } while (task == Unselected);

    return task;
}

const ModelInterface& TextUserInterface::select_model(const std::vector<ModelInterface>& availableInterfaces) const {
    std::cout << "Choose a model" << std::endl;
    int i = 0;
    for (const ModelInterface& mModel : availableInterfaces) {
        std::cout << "\t\t"  << i << mModel.get_name()  << std::endl;
    }
    while(true) {
        std::string input;
        std::cin >> input;
        if (input.empty() || input.length() > 1) {
            std::cout << "Please enter 1 or 2!";
            continue;
        } else {
            for (i = 0; i < availableInterfaces.size(); i++) {
                if (input == std::to_string(i)) {
                    return (availableInterfaces[i]);
                }
            }
        }
    }
}

const ModelInterface& TextUserInterface::select_model() const {
    Model model = Uns;
    std::cout << "Choose a model" << std::endl
              << "\t\t 1. WA" << std::endl
              << "\t\t 2. DE" << std::endl;

    do {
        std::string input;
        std::cin >> input;
        if (input.empty() || input.length() > 1 ||
            (!(input == "1") && !(input == "2"))) {
            std::cout << "Please enter 1, 2 or 3!";
            continue;
        } else {
            model = Model(std::stoi(input));
        }
    } while (model == Uns);
    const ModelInterface& result = *(new ModelInterface());
    switch (model) {
        case WA: {
            //result = WeightedAutomatonModel();
            break;
        }
        case DE:{
            //result = DifferentialEquationModel();
            break;
        }
        case Uns:
            break;
    }
    return result;
}

UserInterface::IOMethod TextUserInterface::select_io_method() const {
    IOMethod io_method;
    std::cout << "Choose an input method" << std::endl
              << "\t\t 1. File" << std::endl
              << "\t\t 2. Stdin" << std::endl;

    do {
        std::string input;
        std::cin >> input;
        if (input.empty() || input.length() > 1 ||
            (!(input == "1") && !(input == "2"))) {
            std::cout << "Please enter 1, 2 or 3!";
            continue;
        } else {
            io_method = IOMethod(std::stoi(input));
        }
    } while (io_method == Unse);

    return io_method;
}

std::stringstream TextUserInterface::file_input() const {
    bool pick = false;
    std::string line;
    do {
        std::cout << "Please enter the file name where the input is to be taken from" << std::endl;
        std::getline(std::cin, line);
        std::filesystem::path p = std::filesystem::path(line);
        if (p.empty() || !p.has_filename() || !std::filesystem::exists(p)) {
            std::cout << "Please enter a valid path!" << std::endl;
            continue;
        } else {
            pick = true;
        }
    } while (!pick);
    std::ifstream tempStream;
    tempStream.open(line);
    std::stringstream result;
    result << tempStream.rdbuf();
    tempStream.close();
    return result;
}

std::stringstream TextUserInterface::stdin_input() const {

}


const std::string& TextUserInterface::set_output_destination() const {

}

void TextUserInterface::display_file(const std::stringstream&) const {

}

void TextUserInterface::display_stdout(const std::stringstream&) const {

}

const ReductionMethodInterface& TextUserInterface::select_reduction_method(const ModelInterface& anInterface) const {

}

const ConversionMethodInterface& TextUserInterface::select_conversion_method(const ModelInterface& anInterface,
        const ModelInterface& modelInterface) const {

}
