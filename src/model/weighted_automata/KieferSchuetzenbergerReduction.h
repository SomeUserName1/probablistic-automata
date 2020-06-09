#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H

#include <string>
#include <memory>
#include <iostream>
#include <random>

#include "../ReductionMethodInterface.h"
#include "WeightedAutomatonInstance.h"

template <Matrix M>
class KieferSchuetzenbergerReduction : public ReductionMethodInterface {
public:
    KieferSchuetzenbergerReduction();
    ~KieferSchuetzenbergerReduction() override;

    std::string get_name() const override {
        return "Random Basis Schützenberger Reduction";
    }

    std::shared_ptr<RepresentationInterface> reduce(std::shared_ptr<RepresentationInterface> &waInstance)
    const override {
        return reduce(waInstance, 300);
    }

    static std::shared_ptr<RepresentationInterface> reduce(std::shared_ptr<RepresentationInterface> &waInstance,
                                                           uint K) {
        auto A = std::dynamic_pointer_cast<WeightedAutomatonInstance<M>>(waInstance);
        auto randomVectors = generate_random_vectors(A, K);
        std::shared_ptr<WeightedAutomatonInstance<M>> minA = forward_reduction(A, randomVectors);
        randomVectors = generate_random_vectors(minA, K);
        minA = backward_reduction(minA, randomVectors);
        return std::move(minA);
    }

    static std::shared_ptr<WeightedAutomatonInstance<M>> backward_reduction(
            std::shared_ptr<WeightedAutomatonInstance<M>> &A, std::vector<Eigen::MatrixXi> randomVectors) {
        auto rhoVectors = calculate_rho_backward_vectors(A, randomVectors);
        Eigen::MatrixXd rankTemp(A->get_states(), 1 + rhoVectors.size());

        rankTemp.col(0) = *(A->get_eta());
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (size_t i = 0; i < rhoVectors.size(); i++) {
            rankTemp.col(static_cast<long>(i + 1)) = rhoVectors[i];
        }
        Eigen::ColPivHouseholderQR<Eigen::Ref<Eigen::MatrixXd>> qr(rankTemp);
        long rank = qr.rank();

        Eigen::MatrixXd backwardBasis(A->get_states(), rank);
        backwardBasis.col(0) = *(A->get_eta());
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (long i = 0; i < rank - 1; i++) {
            backwardBasis.col(i + 1) = rhoVectors[static_cast<size_t>(i)];
        }

        Eigen::MatrixXd temp = *(A->get_alpha()) * backwardBasis;
        auto alphaArrow = std::make_shared<M>(temp);
        auto etaArrow = std::make_shared<M>(MatrixSp(rank, 1));
        (*etaArrow)(0, 0) = 1;

        std::vector<std::shared_ptr<M>> muArrow = {};
        std::mutex muArrowMutex = std::mutex();

#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (size_t i = 0; i < A->get_mu().size(); i++) {
            Eigen::ColPivHouseholderQR<Eigen::MatrixXd> householderX(rank, rank);
            householderX.compute(backwardBasis);
            Eigen::MatrixXd newTemp = householderX.solve(*(A->get_mu()[i]) * backwardBasis);
            auto muXArrow = std::make_shared<M>(newTemp);
            std::lock_guard<std::mutex> guard(muArrowMutex);
            muArrow.push_back(muXArrow);
        }
        return std::make_shared<WeightedAutomatonInstance<M>>(static_cast<uint>(rank), A->get_number_input_characters(),
                alphaArrow, muArrow, etaArrow);
    }

    static std::shared_ptr<WeightedAutomatonInstance<M>> forward_reduction(
            std::shared_ptr<WeightedAutomatonInstance<M>> &A, std::vector<Eigen::MatrixXi> randomVectors) {
        auto rhoVectors = calculate_rho_forward_vectors(A, randomVectors);
        Eigen::MatrixXd rankTemp(1 + rhoVectors.size(), A->get_states());

        rankTemp.row(0) = *(A->get_alpha());
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (size_t i = 0; i < rhoVectors.size(); i++) {
            rankTemp.row(static_cast<long>(i + 1)) = rhoVectors[i];
        }
        Eigen::ColPivHouseholderQR<Eigen::Ref<Eigen::MatrixXd>> qr(rankTemp);
        long rank = qr.rank();

        Eigen::MatrixXd forwardBasis(rank, A->get_states());
        forwardBasis.row(0) = *(A->get_alpha());
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (long i = 0; i < rank - 1; i++) {
            forwardBasis.row(i + 1) = rhoVectors[static_cast<size_t>(i)];
        }

        Eigen::MatrixXd temp = forwardBasis * *(A->get_eta());
        auto etaArrow = std::make_shared<M>(temp);
        auto alphaArrow = std::make_shared<M>(MatrixSp(1, rank));
        (*alphaArrow)(0, 0) = 1;

        std::vector<std::shared_ptr<M>> muArrow = {};
        std::mutex muArrowMutex = std::mutex();

#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (size_t i = 0; i < A->get_mu().size(); i++) {
            Eigen::ColPivHouseholderQR<Eigen::MatrixXd> householderX(rank, rank);
            // x*A = b <=> A.transpose() * z = b.transpose(); x = z.transpose()
            // => x = (housholder(A.transpose()).solve(b.transpose())).transpose()
            householderX.compute(forwardBasis.transpose());
            Eigen::MatrixXd newTemp = householderX.solve((forwardBasis * *(A->get_mu()[i])).transpose()).transpose();
            auto muXArrow = std::make_shared<M>(newTemp);m
            std::lock_guard<std::mutex> guard(muArrowMutex);
            muArrow.push_back(muXArrow);
        }
        return std::make_shared<WeightedAutomatonInstance<M>>(static_cast<uint>(rank), A->get_number_input_characters(),
                alphaArrow, muArrow, etaArrow);

    }

    static inline std::vector<Eigen::MatrixXi> generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance<M>> &A,
            uint K = 300, bool seeded = false, int seed = 0) {
        std::mt19937 rng;
        if (seeded) {
            rnd = std::mt19937(seed)
        } else {
            std::random_device rd;
            rng = std::mt19937(rd)
        }
        std::uniform_int_distribution<> uniform(1, static_cast<int>(A->get_states() * A->get_states() * K));
        std::vector<Eigen::MatrixXi> randV;

        for (uint i = 0; i < A->get_states(); i++) {
            auto vect = Eigen::MatrixXi(A->get_number_input_characters(), A->get_states());
            for (uint j = 0; j < A->get_number_input_characters(); j++) {
                for (uint k = 0; k < A->get_states(); k++) {
                    vect(j, k) = uniform(rng);
                }
            }
            randV.push_back(vect);
        }
        return randV;
    }

    static std::vector<Eigen::VectorXd> calculate_rho_backward_vectors(std::shared_ptr<WeightedAutomatonInstance<M>> &A,
                                                                       std::vector<Eigen::MatrixXi> randomVectors) {
        auto sigmaK = generate_words_backwards(A, A->get_states());
        std::vector<Eigen::VectorXd> result = {};
        std::mutex resultMutex = std::mutex();
#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (size_t j = 0; j < randomVectors.size(); j++) {
            Eigen::VectorXd vI = Eigen::MatrixXd::Zero(A->get_states(), 1);
#pragma omp parallel for num_threads(THREADS) if(!TEST)
            for (size_t i = 0; i < sigmaK.size(); i++) {
                vI += (*std::get<0>(sigmaK[i]) * get_word_factor(std::get<1>(sigmaK[i]), randomVectors[j]));
            }
            std::lock_guard<std::mutex> guard(resultMutex);
            result.push_back(vI.transpose());
        }
        return result;
    }

    static std::vector<Eigen::RowVectorXd> calculate_rho_forward_vectors(std::shared_ptr<WeightedAutomatonInstance<M>> &A,
                                                                         std::vector<Eigen::MatrixXi> randomVectors)  {
        auto sigmaK = generate_words_forwards(A, A->get_states());
        std::vector<Eigen::RowVectorXd> result = {};
        std::mutex resultMutex = std::mutex();

#pragma omp parallel for num_threads(THREADS) if(!TEST)
        for (size_t j = 0; j < randomVectors.size(); j++) {
            Eigen::RowVectorXd vI = Eigen::MatrixXd::Zero(1, A->get_states());
#pragma omp parallel for num_threads(THREADS) if(!TEST)
            for (size_t i = 0; i < sigmaK.size(); i++) {
                vI += (*std::get<0>(sigmaK[i]) * get_word_factor(std::get<1>(sigmaK[i]), randomVectors[j]));
            }
            std::lock_guard<std::mutex> guard(resultMutex);
            result.push_back(vI);
        }
        return result;
    }

    static inline int get_word_factor(std::vector<uint> word, Eigen::MatrixXi
    randVector) {
        int result = 1;
        for (uint i = 0; i < word.size(); i++) {
            result *= randVector(word[i], i);
        }
        return result;
    }

    static std::vector<std::tuple<std::shared_ptr<Eigen::RowVectorXd>, std::vector<uint>>> generate_words_forwards(
            std::shared_ptr<WeightedAutomatonInstance<M>> &A, uint k) {
        std::vector<std::tuple<std::shared_ptr<Eigen::RowVectorXd>, std::vector<uint>>> result;
        std::mutex resultMutex = std::mutex();

        if (k == 1) {
            result = {};
#pragma omp parallel for num_threads(THREADS) if(!TEST)
            for (size_t i = 0; i < A->get_mu().size(); i++) {
                auto resultVect = std::make_shared<Eigen::RowVectorXd>(A->get_states());
                *resultVect = *(A->get_alpha()) * *(A->get_mu()[i]);
                if (!resultVect->isZero()) {
                    std::lock_guard<std::mutex> guard(resultMutex);
                    result.emplace_back(resultVect, std::vector({static_cast<uint>(i)}));
                }
            }
        } else {
            result = generate_words_forwards(A, k - 1);
            auto iteratorCopy(result);

#pragma omp parallel for num_threads(THREADS) if(!TEST)
            for (size_t j = 0; j < iteratorCopy.size(); j++) {
                if (std::get<1>(iteratorCopy[j]).size() == k - 1) {
#pragma omp parallel for num_threads(THREADS) if(!TEST)
                    for (size_t i = 0; i < A->get_mu().size(); i++) {
                        auto resultVect = std::make_shared<Eigen::RowVectorXd>(A->get_states());
                        *resultVect = *(std::get<0>(iteratorCopy[j])) * *(A->get_mu()[i]);
                        if (!resultVect->isZero()) {
                            auto temp = std::get<1>(iteratorCopy[j]);
                            temp.push_back(static_cast<uint>(i));
                            std::lock_guard<std::mutex> guard(resultMutex);
                            result.emplace_back(resultVect, temp);
                        }
                    }
                }
            }
        }
        return result;
    }


    static std::vector<std::tuple<std::shared_ptr<Eigen::VectorXd>, std::vector<uint>>> generate_words_backwards(
            std::shared_ptr<WeightedAutomatonInstance<M>> &A, uint k) {
        std::vector<std::tuple<std::shared_ptr<Eigen::VectorXd>, std::vector<uint>>> result;
        std::mutex resultMutex = std::mutex();

        if (k == 1) {
            result = {};
#pragma omp parallel for num_threads(THREADS) if(!TEST)
            for (size_t i = 0; i < A->get_mu().size(); i++) {
                auto resultVect = std::make_shared<Eigen::VectorXd>(A->get_states());
                *resultVect = *(A->get_mu()[i]) * *(A->get_eta());
                if (!resultVect->isZero()) {
                    std::lock_guard<std::mutex> guard(resultMutex);
                    result.emplace_back(resultVect, std::vector({static_cast<uint>(i)}));
                }

            }
        } else {
            result = generate_words_backwards(A, k - 1);
            auto iteratorCopy(result);

#pragma omp parallel for num_threads(THREADS) if(!TEST)
            for (size_t j = 0; j < iteratorCopy.size(); j++) {
                if (std::get<1>(iteratorCopy[j]).size() == k - 1) {
#pragma omp parallel for num_threads(THREADS) if(!TEST)
                    for (size_t i = 0; i < A->get_mu().size(); i++) {
                        auto resultVect = std::make_shared<Eigen::VectorXd>(A->get_states());
                        *resultVect = *(A->get_mu()[i]) * *(std::get<0>(iteratorCopy[j]));
                        if (!resultVect->isZero()) {
                            auto temp = std::get<1>(iteratorCopy[j]);
                            temp.insert(temp.begin(), static_cast<uint>(i));
                            std::lock_guard<std::mutex> guard(resultMutex);
                            result.emplace_back(resultVect, temp);
                        }
                    }
                }
            }
        }
        return result;
    }
};

template<Matrix M>
KieferSchuetzenbergerReduction<M>::KieferSchuetzenbergerReduction() {}

template<Matrix M>
KieferSchuetzenbergerReduction<M>::~KieferSchuetzenbergerReduction() {}

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_KIEFERSCHUETZENBERGERREDUCTION_H
