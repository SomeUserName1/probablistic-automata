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

    static inline std::vector<Eigen::MatrixXi> generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
            int K = 300) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<> uniform(1, A->get_states() * K);
        std::vector<Eigen::MatrixXi> randV;

        for (int i = 0; i < A->get_states(); i++) {
            auto vect = Eigen::MatrixXi(A->get_number_input_characters(), A->get_states());
            for (int j = 0; j < A->get_number_input_characters(); j++) {
                for (int k = 0; k < A->get_states(); k++) {
                    vect(j, k) = uniform(rng);
                }
            }
            randV.push_back(vect);
        }
        return randV;
    }

    static std::vector<Eigen::VectorXd> calculate_rho_backward_vectors(std::shared_ptr<WeightedAutomatonInstance> &,
                                                                       std::vector<Eigen::MatrixXi>);

    static std::vector<Eigen::RowVectorXd> calculate_rho_forward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                                         std::vector<Eigen::MatrixXi>);

    static inline int get_word_factor(std::vector<uint> word, Eigen::MatrixXi
    randVector) {
        int result = 1;
        for (uint i = 0; i < word.size(); i++) {
            result *= randVector(word[i], i);
        }
        return result;
    }

    static std::vector<std::tuple<std::shared_ptr<Eigen::RowVectorXd>, std::vector<uint>>> generate_words_forwards(
            std::shared_ptr<WeightedAutomatonInstance> &A, int k);

    static std::vector<std::tuple<std::shared_ptr<Eigen::VectorXd>, std::vector<uint>>> generate_words_backwards(
            std::shared_ptr<WeightedAutomatonInstance> &A, int k);


};

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
