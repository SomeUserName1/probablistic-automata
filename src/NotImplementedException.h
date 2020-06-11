#include <stdexcept>

class NotImplementedException : public std::logic_error {
public:
  NotImplementedException();

  ~NotImplementedException() override = default;

  [[nodiscard]] auto what() const noexcept -> const char * override;
};
