#include "NotImplementedException.h"

NotImplementedException::NotImplementedException()
    : std::logic_error("Function not yet implemented.") {}

auto NotImplementedException::what() const noexcept -> const char * {
  return "The function has not been implemented yet or is not supposed to be "
         "implemented";
}
