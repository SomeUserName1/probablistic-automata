//
// Created by someusername on 19.05.20.
//

#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H

#include <string>
#include <memory>
#include <iostream>
#include <random>

#include "ReductionMethodInterface.h"
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

        std::vector<Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>> generate_random_vectors
        (std::shared_ptr<WeightedAutomatonInstance> &A,
                int K) const;

    std::vector<Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>> calculate_rho_backward_vectors(
            std::shared_ptr<WeightedAutomatonInstance> &sharedPtr,
            std::vector<Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>> vector) const;

    std::vector<std::tuple<Eigen::Matrix<float, -1, 1>, std::vector<uint>>>
    generate_words(std::shared_ptr<WeightedAutomatonInstance> &A, int k)
        const;
};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
