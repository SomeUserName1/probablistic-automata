#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H

class ConversionMethodInterface {
public:
    virtual ~ConversionMethodInterface() = default;

    virtual std::string get_left_model_name() const = 0;

    virtual std::string get_right_model_name() const = 0;

    virtual RepresentationInterface convert_left_to_right(RepresentationInterface) const = 0;

    virtual RepresentationInterface convert_right_to_left(RepresentationInterface) const = 0;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H
