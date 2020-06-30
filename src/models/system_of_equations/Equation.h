#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_EQUATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_EQUATION_H

#include "../RepresentationInterface.h"

class Equation : public RepresentationInterface {
public:
  ~Equation() override = 0;
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_EQUATION_H
