#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H

#include <string>
#include <memory>
#include <iostream>
#include <random>

#include "../ReductionMethodInterface.h"
#include "WeightedAutomatonInstance.h"

class KieferSchuetzenbergerReduction : public ReductionMethodInterface {
public:
    ~KieferSchuetzenbergerReduction() override = default;

    [[nodiscard]] std::string get_name() const override;

    [[nodiscard]] std::shared_ptr<RepresentationInterface> reduce(std::shared_ptr<RepresentationInterface> &)
    const override;

    static std::shared_ptr<RepresentationInterface> reduce(std::shared_ptr<RepresentationInterface> &waInstance,
                                                           int K);

    static std::shared_ptr<WeightedAutomatonInstance> backward_reduction(std::shared_ptr<WeightedAutomatonInstance>
                                                                         &A,
                                                                         std::vector<Eigen::MatrixXi> randomVectors);

    static std::shared_ptr<WeightedAutomatonInstance> forward_reduction(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                                        std::vector<Eigen::MatrixXi> randomVectors);

    static std::vector<Eigen::MatrixXi> generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &A, int K);

    static std::vector<Eigen::VectorXd> calculate_rho_backward_vectors(std::shared_ptr<WeightedAutomatonInstance> &,
                                                                       std::vector<Eigen::MatrixXi>);

    static std::vector<Eigen::RowVectorXd> calculate_rho_forward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                                         std::vector<Eigen::MatrixXi>);

    static int get_word_factor(std::vector<uint> word, Eigen::Matrix<int, -1, -1> randVector);

    static std::vector<std::tuple<std::shared_ptr<Eigen::RowVectorXd>, std::vector<uint>>> generate_words_forwards(
            std::shared_ptr<WeightedAutomatonInstance> &A, int k);

    static std::vector<std::tuple<std::shared_ptr<Eigen::VectorXd>, std::vector<uint>>> generate_words_backwards(
            std::shared_ptr<WeightedAutomatonInstance> &A, int k);


};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
