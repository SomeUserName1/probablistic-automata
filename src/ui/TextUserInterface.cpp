#include "TextUserInterface.h"

UserInterface::Task TextUserInterface::select_task() const {
    Task task = Unselected;
    std::cout << "Hello," << std::endl
              << "this is a program to automatically minimize models of stochastic dynamical systems like automata"
              << " or differential equations." << std::endl
              << "What do you want to do?" << std::endl
              << "\t\t 1. Reduce an example" << std::endl
              << "\t\t 2. Perform a benchmark" << std::endl
              << "\t\t 3. Convert an example from one model to another" << std::endl
              << "\t\t 4. to exit" << std::endl;

    do {
        std::string input("");
        std::cin >> input;
        if (input.empty() || input.length() > 1 ||
            (input != "1" && input != "2" && input != "3" && input != "4")) {
            std::cout << "Please enter 1, 2 or 3!";
            continue;
        } else {
            task = Task(std::stoi(input));
        }
    } while (task == Unselected);

    return task;
}

std::shared_ptr<ModelInterface>
TextUserInterface::select_model(const std::vector<std::shared_ptr<ModelInterface>> &availableModels) const {
    std::cout << "Choose a model" << std::endl;
    uint i = 0;
    for (const auto &mModel : availableModels) {
        std::cout << "\t\t" << i << ". " << mModel->get_name() << std::endl;
        ++i;
    }
    while (true) {
        std::string input("");
        std::cin >> input;
        if (input.empty() || input.length() > 1) {
            std::cout << "Please enter 0 or 1!";
            continue;
        } else {
            for (i = 0; i < availableModels.size(); i++) {
                if (input == std::to_string(i)) {
                    return availableModels[i];
                }
            }
        }
    }
}

UserInterface::IOMethod TextUserInterface::select_io_method(bool in) const {
    IOMethod io_method = Unse;
    std::cout << "Choose an ";
    if (in) {
        std::cout << "input";
    } else {
        std::cout << "output";
    }
    std::cout << " method" << std::endl
              << "\t\t 1. File" << std::endl;
    if (in) {
        std::cout << "\t\t 2. Stdin" << std::endl;
    } else {
        std::cout << "\t\t 2. Stdout" << std::endl;
    }

    do {
        std::string input("");
        std::cin >> input;
        if (input.empty() || input.length() > 1 ||
            (input != "1" && input != "2")) {
            std::cout << "Please enter 1, 2 or 3!";
            continue;
        } else {
            io_method = IOMethod(std::stoi(input));
        }
    } while (io_method == Unse);

    return io_method;
}

std::string TextUserInterface::file_input() const {
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
    return read_file(line);
}

std::string TextUserInterface::stdin_input(std::shared_ptr<ModelInterface> &selectedModel) const {
    std::cout << "Please enter the input!" << std::endl;
    std::cout << selectedModel->get_representation_description();
    std::string inputLine;
    std::stringstream ss;
    bool emptyBefore = false;
    while (std::getline(std::cin, inputLine)) {
        // when we get two consecutive empty lines, terminate the input
        if (inputLine.empty()) {
            if (emptyBefore) {
                break;
            } else {
                emptyBefore = true;
            }
        }
        ss << inputLine;
    }
    return ss.str();
}


std::string TextUserInterface::set_output_destination() const {
    bool pick = false;
    std::string line;
    do {
        std::cout << "Please enter the file name where the output shall go" << std::endl;
        std::getline(std::cin, line);
        std::filesystem::path p = std::filesystem::path(line);
        if (p.empty() || !p.has_filename()) {
            std::cout << "Please enter a valid path!" << std::endl;
            continue;
        } else {
            pick = true;
        }
    } while (!pick);
    return line;
}

void TextUserInterface::display(const std::string &output) const {
    std::cout << output << std::endl;
}


std::shared_ptr<ReductionMethodInterface> TextUserInterface::select_reduction_method(
        std::shared_ptr<ModelInterface> &selectedModel) const {
    std::cout << "Choose a reduction method" << std::endl;
    uint i = 0;
    for (const auto &reduction : selectedModel->get_reduction_methods()) {
        std::cout << "\t\t" << i << ". " << reduction->get_name() << std::endl;
        ++i;
    }
    while (true) {
        std::string input("");
        std::cin >> input;
        if (input.empty() || input.length() > 1) {
            std::cout << "Please enter 1 or 2!";
            continue;
        } else {
            for (i = 0; i < selectedModel->get_reduction_methods().size(); i++) {
                if (input == std::to_string(i)) {
                    return selectedModel->get_reduction_methods()[i];
                }
            }
        }
    }
}

std::shared_ptr<ConversionMethodInterface>
TextUserInterface::select_conversion_method(std::shared_ptr<ModelInterface> &model) const {
    std::cout << "Choose a conversion method" << std::endl;
    uint i = 0;
    for (const auto &conversion : model->get_conversion_methods()) {
        std::cout << "\t\t" << i << ". " << conversion->get_left_model_name() << " to " <<
                  conversion->get_right_model_name() << std::endl;
        ++i;
    }
    while (true) {
        std::string input("");
        std::cin >> input;
        if (input.empty() || input.length() > 1) {
            std::cout << "Please enter 1 or 2!";
            continue;
        } else {
            for (i = 0; i < model->get_conversion_methods().size(); i++) {
                if (input == std::to_string(i)) {
                    return model->get_conversion_methods()[i];
                }
            }
        }
    }
}