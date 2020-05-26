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

        [[nodiscard]] std::shared_ptr<RepresentationInterface> reduce(std::shared_ptr<RepresentationInterface>&)
        const override;

        std::shared_ptr<RepresentationInterface> reduce(std::shared_ptr<RepresentationInterface> &waInstance,
                int K) const;

        std::shared_ptr<WeightedAutomatonInstance> backward_reduction(std::shared_ptr<WeightedAutomatonInstance> &A,
                int K) const;

        std::shared_ptr<WeightedAutomatonInstance> forward_reduction(std::shared_ptr<WeightedAutomatonInstance> &A,
                int K) const;

        std::vector<Eigen::MatrixXi> generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &A, int K)
            const;

        std::vector<Eigen::VectorXf> calculate_rho_backward_vectors(std::shared_ptr<WeightedAutomatonInstance> &, int)
            const;

        int get_word_factor(std::vector<uint> word, Eigen::Matrix<int, -1, -1> randVector) const;

        std::vector<std::tuple<std::shared_ptr<Eigen::RowVectorXf>, std::vector<uint>>> generate_words_forwards(
                std::shared_ptr<WeightedAutomatonInstance> &A, int k) const;

        std::vector<std::tuple<std::shared_ptr<Eigen::VectorXf>, std::vector<uint>>> generate_words_backwards(
                std::shared_ptr<WeightedAutomatonInstance> &A, int k) const;

        std::vector<Eigen::RowVectorXf> calculate_rho_forward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A, int K)
            const;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
