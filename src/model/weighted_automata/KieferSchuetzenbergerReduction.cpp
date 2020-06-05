#include "KieferSchuetzenbergerReduction.h"

std::string KieferSchuetzenbergerReduction::get_name() const {
    return "Random Basis Schützenberger Reduction";
}

std::shared_ptr<RepresentationInterface> KieferSchuetzenbergerReduction::reduce(
        std::shared_ptr<RepresentationInterface> &waInstance) const {
    return reduce(waInstance, 300);
}

std::shared_ptr<RepresentationInterface> KieferSchuetzenbergerReduction::reduce(
        std::shared_ptr<RepresentationInterface> &waInstance, uint K) {
    auto A = std::dynamic_pointer_cast<WeightedAutomatonInstance>(waInstance);

    auto randomVectors = generate_random_vectors(A, K);
    std::shared_ptr<WeightedAutomatonInstance> minA = forward_reduction(A, randomVectors);
    randomVectors = generate_random_vectors(minA, K);
    minA = backward_reduction(minA, randomVectors);
    return std::move(minA);
}

std::shared_ptr<WeightedAutomatonInstance> KieferSchuetzenbergerReduction::backward_reduction(
        std::shared_ptr<WeightedAutomatonInstance> &A, std::vector<Eigen::MatrixXi> randomVectors) {
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

    auto alphaArrow = std::make_shared<Eigen::RowVectorXd>(rank);
    *alphaArrow = *(A->get_alpha()) * backwardBasis;
    auto etaArrow = std::make_shared<Eigen::VectorXd>(Eigen::MatrixXd::Zero(rank, 1));
    (*etaArrow)(0, 0) = 1;

    std::vector<std::shared_ptr<Eigen::MatrixXd>> muArrow = {};
    std::mutex muArrowMutex = std::mutex();

    #pragma omp parallel for num_threads(THREADS) if(!TEST) 
    for (size_t i = 0; i < A->get_mu().size(); i++) {
        Eigen::ColPivHouseholderQR<Eigen::MatrixXd> householderX(rank, rank);
        auto muXArrow = std::make_shared<Eigen::MatrixXd>(rank, rank);
        householderX.compute(backwardBasis);
        *muXArrow = householderX.solve(*(A->get_mu()[i]) * backwardBasis);
        std::lock_guard<std::mutex> guard(muArrowMutex);
        muArrow.push_back(muXArrow);
    }
    return std::make_shared<WeightedAutomatonInstance>(rank, A->get_number_input_characters(), alphaArrow, muArrow,
                                                       etaArrow);
}

std::shared_ptr<WeightedAutomatonInstance> KieferSchuetzenbergerReduction::forward_reduction(
        std::shared_ptr<WeightedAutomatonInstance> &A, std::vector<Eigen::MatrixXi> randomVectors) {
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

    auto etaArrow = std::make_shared<Eigen::VectorXd>(rank);
    *etaArrow = forwardBasis * *(A->get_eta());
    auto alphaArrow = std::make_shared<Eigen::RowVectorXd>(Eigen::MatrixXd::Zero(1, rank));
    (*alphaArrow)(0, 0) = 1;

    std::vector<std::shared_ptr<Eigen::MatrixXd>> muArrow = {};
    std::mutex muArrowMutex = std::mutex();

    #pragma omp parallel for num_threads(THREADS) if(!TEST) 
    for (size_t i = 0; i < A->get_mu().size(); i++) {
        Eigen::ColPivHouseholderQR<Eigen::MatrixXd> householderX(rank, rank);
        auto muXArrow = std::make_shared<Eigen::MatrixXd>(rank, rank);
        // x*A = b <=> A.transpose() * z = b.transpose(); x = z.transpose()
        // => x = (housholder(A.transpose()).solve(b.transpose())).transpose()
        householderX.compute(forwardBasis.transpose());
        *muXArrow = (householderX.solve((forwardBasis * *(A->get_mu()[i])).transpose())).transpose();
        std::lock_guard<std::mutex> guard(muArrowMutex);
        muArrow.push_back(muXArrow);
    }
    return std::make_shared<WeightedAutomatonInstance>(rank, A->get_number_input_characters(), alphaArrow, muArrow,
                                                       etaArrow);

}

std::vector<Eigen::VectorXd>
KieferSchuetzenbergerReduction::calculate_rho_backward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
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

std::vector<Eigen::RowVectorXd>
KieferSchuetzenbergerReduction::calculate_rho_forward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                              std::vector<Eigen::MatrixXi> randomVectors) {
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

std::vector<std::tuple<std::shared_ptr<Eigen::VectorXd>, std::vector<uint>>>
KieferSchuetzenbergerReduction::generate_words_backwards(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                         uint k) {
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

std::vector<std::tuple<std::shared_ptr<Eigen::RowVectorXd>, std::vector<uint>>>
KieferSchuetzenbergerReduction::generate_words_forwards(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                        uint k) {
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
