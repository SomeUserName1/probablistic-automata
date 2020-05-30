#include <stdexcept>

class NotImplementedException : public std::logic_error {
public:
    NotImplementedException();

    ~NotImplementedException() override = default;

    const char *what() const noexcept override;
};
