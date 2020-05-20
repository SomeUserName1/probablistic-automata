#include "NotImplementedException.h"

NotImplementedException::NotImplementedException(): std::logic_error("Function not yet implemented.") {}

const char *NotImplementedException::what() const noexcept {
    return "The function has not been implemented yet or is not supposed to be implemented";
}
