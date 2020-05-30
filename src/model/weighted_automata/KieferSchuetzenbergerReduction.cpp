#include "KieferSchuetzenbergerReduction.h"

std::string KieferSchuetzenbergerReduction::get_name() const {
    return "Random Basis Schützenberger Reduction";
}

std::shared_ptr<RepresentationInterface> KieferSchuetzenbergerReduction::reduce(
        std::shared_ptr<RepresentationInterface> &waInstance) const {
    return reduce(waInstance, 300);
}

std::shared_ptr<RepresentationInterface> KieferSchuetzenbergerReduction::reduce(
        std::shared_ptr<RepresentationInterface> &waInstance, int K) {
    auto A = std::dynamic_pointer_cast<WeightedAutomatonInstance>(waInstance);

    auto randomVectors = generate_random_vectors(A, K);
    std::shared_ptr<WeightedAutomatonInstance> minA = forward_reduction(A, randomVectors);
    randomVectors = generate_random_vectors(minA, K);
    minA = backward_reduction(minA, randomVectors);
    return minA;
}

std::shared_ptr<WeightedAutomatonInstance> KieferSchuetzenbergerReduction::backward_reduction(
        std::shared_ptr<WeightedAutomatonInstance> &A, std::vector<Eigen::MatrixXi> randomVectors) {
    auto rhoVectors = calculate_rho_backward_vectors(A, randomVectors);
    Eigen::MatrixXd rankTemp(A->get_states(), 1 + rhoVectors.size());

    rankTemp.col(0) = *(A->get_eta());
    int i = 1;
    for (auto rhoVector : rhoVectors) {
        rankTemp.col(i) = rhoVector;
    }
    Eigen::ColPivHouseholderQR<Eigen::Ref<Eigen::MatrixXd>> qr(rankTemp);
    int rank = qr.rank();

    Eigen::MatrixXd backwardBasis(A->get_states(), rank);
    backwardBasis.col(0) = *(A->get_eta());
    for (i = 1; i < rank; i++) {
        backwardBasis.col(i) = rhoVectors[i - 1];
    }

    auto alphaArrow = std::make_shared<Eigen::RowVectorXd>(rank);
    *alphaArrow = *(A->get_alpha()) * backwardBasis;
    auto etaArrow = std::make_shared<Eigen::VectorXd>(Eigen::MatrixXd::Zero(rank, 1));
    (*etaArrow)(0, 0) = 1;
    std::vector<std::shared_ptr<Eigen::MatrixXd>> muArrow = {};

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> householderX(rank, rank);

    for (auto muX : A->get_mu()) {
        auto muXArrow = std::make_shared<Eigen::MatrixXd>(rank, rank);
        householderX.compute(backwardBasis);
        *muXArrow = householderX.solve(*muX * backwardBasis);
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
    int i = 1;
    for (auto rhoVector : rhoVectors) {
        rankTemp.row(i) = rhoVector;
        i++;
    }
    Eigen::ColPivHouseholderQR<Eigen::Ref<Eigen::MatrixXd>> qr(rankTemp);
    int rank = qr.rank();

    Eigen::MatrixXd forwardBasis(rank, A->get_states());
    forwardBasis.row(0) = *(A->get_alpha());
    for (i = 1; i < rank; i++) {
        forwardBasis.row(i) = rhoVectors[i - 1];
    }

    auto etaArrow = std::make_shared<Eigen::VectorXd>(rank);
    *etaArrow = forwardBasis * *(A->get_eta());
    auto alphaArrow = std::make_shared<Eigen::RowVectorXd>(Eigen::MatrixXd::Zero(1, rank));
    (*alphaArrow)(0, 0) = 1;
    std::vector<std::shared_ptr<Eigen::MatrixXd>> muArrow = {};

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> householderX(rank, rank);

    for (auto muX : A->get_mu()) {
        auto muXArrow = std::make_shared<Eigen::MatrixXd>(rank, rank);
        // x*A = b <=> A.transpose() * z = b.transpose(); x = z.transpose()
        // => x = (housholder(A.transpose()).solve(b.transpose())).transpose()
        householderX.compute(forwardBasis.transpose());
        *muXArrow = (householderX.solve((forwardBasis * *muX).transpose())).transpose();
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

    for (auto randomVector : randomVectors) {
        Eigen::VectorXd vI = Eigen::MatrixXd::Zero(A->get_states(), 1);
        for (auto word : sigmaK) {
            vI = vI + (*std::get<0>(word) * get_word_factor(std::get<1>(word), randomVector));
        }
        result.push_back(vI.transpose());
    }
    return result;
}

std::vector<Eigen::RowVectorXd>
KieferSchuetzenbergerReduction::calculate_rho_forward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                              std::vector<Eigen::MatrixXi> randomVectors) {
    auto sigmaK = generate_words_forwards(A, A->get_states());
    std::vector<Eigen::RowVectorXd> result = {};

    for (auto randomVector : randomVectors) {
        Eigen::RowVectorXd vI = Eigen::MatrixXd::Zero(1, A->get_states());
        for (auto word : sigmaK) {
            vI = vI + (*std::get<0>(word) * get_word_factor(std::get<1>(word), randomVector));
        }
        result.push_back(vI);
    }
    return result;
}

std::vector<std::tuple<std::shared_ptr<Eigen::VectorXd>, std::vector<uint>>>
KieferSchuetzenbergerReduction::generate_words_backwards(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                         int k) {
    std::vector<std::tuple<std::shared_ptr<Eigen::VectorXd>, std::vector<uint>>> result;
    std::vector<uint> temp;
    if (k == 1) {
        result = {};
        for (uint i = 0; i < A->get_mu().size(); i++) {
            auto resultVect = std::make_shared<Eigen::VectorXd>(A->get_states());
            *resultVect = *(A->get_mu()[i]) * *(A->get_eta());
            if (!resultVect->isZero()) {
                temp = {i};
                result.emplace_back(resultVect, temp);
            }

        }
    } else {
        result = generate_words_backwards(A, k - 1);
        auto iteratorCopy(result);
        for (auto &element : iteratorCopy) {
            if ((int) std::get<1>(element).size() == k - 1) {
                for (uint i = 0; i < A->get_mu().size(); i++) {
                    auto resultVect = std::make_shared<Eigen::VectorXd>(A->get_states());
                    *resultVect = *(A->get_mu()[i]) * *(std::get<0>(element));
                    if (!resultVect->isZero()) {
                        temp = std::get<1>(element);
                        temp.insert(temp.begin(), i);
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
                                                        int k) {
    std::vector<std::tuple<std::shared_ptr<Eigen::RowVectorXd>, std::vector<uint>>> result;
    std::vector<uint> temp;
    if (k == 1) {
        result = {};
        for (uint i = 0; i < A->get_mu().size(); i++) {
            auto resultVect = std::make_shared<Eigen::RowVectorXd>(A->get_states());
            *resultVect = *(A->get_alpha()) * *(A->get_mu()[i]);
            if (!resultVect->isZero()) {
                temp = {i};
                result.emplace_back(resultVect, temp);
            }
        }
    } else {
        result = generate_words_forwards(A, k - 1);
        auto iteratorCopy(result);
        for (auto &element : iteratorCopy) {
            if ((int) std::get<1>(element).size() == k - 1) {
                for (uint i = 0; i < A->get_mu().size(); i++) {
                    auto resultVect = std::make_shared<Eigen::RowVectorXd>(A->get_states());
                    *resultVect = *(std::get<0>(element)) * *(A->get_mu()[i]);
                    if (!resultVect->isZero()) {
                        temp = std::get<1>(element);
                        temp.push_back(i);
                        result.emplace_back(resultVect, temp);
                    }
                }
            }
        }
    }
    return result;
}

inline std::vector<Eigen::MatrixXi>
KieferSchuetzenbergerReduction::generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &A, int K = 300) {
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

inline int KieferSchuetzenbergerReduction::get_word_factor(std::vector<uint> word, Eigen::MatrixXi randVector) {
    int result = 1;
    for (uint i = 0; i < word.size(); i++) {
        result *= randVector(word[i], i);
    }
    return result;
}
