#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H

class ConversionMethodInterface{
    public:
        virtual RepresentationInterface convert_left_to_right(RepresentationInterface) = 0;
        virtual RepresentationInterface convert_right_to_left(RepresentationInterface) = 0;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H
