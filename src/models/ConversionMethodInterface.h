#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H

#include "RepresentationInterface.h"

class ConversionMethodInterface {
public:
  virtual ~ConversionMethodInterface();
  [[nodiscard]] virtual auto get_left_model_name() const -> std::string = 0;
  [[nodiscard]] virtual auto get_right_model_name() const -> std::string = 0;
  [[nodiscard]] virtual auto
      convert_left_to_right(RepresentationInterface) const
      -> RepresentationInterface = 0;
  [[nodiscard]] virtual auto
      convert_right_to_left(RepresentationInterface) const
      -> RepresentationInterface = 0;
};
#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H
