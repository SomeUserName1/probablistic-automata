#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H

class ConversionMethodInterface{
    public:
        virtual std::string get_left_model_name() const;
        virtual std::string get_right_model_name() const;
        virtual RepresentationInterface convert_left_to_right(RepresentationInterface) const;
        virtual RepresentationInterface convert_right_to_left(RepresentationInterface) const;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_MODELCONVERSIONINTERFACE_H
