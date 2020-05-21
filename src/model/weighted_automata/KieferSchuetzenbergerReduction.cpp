#include "KieferSchuetzenbergerReduction.h"

std::string KieferSchuetzenbergerReduction::get_name() const {
    return "Random Basis Schützenberger Reduction";
}

std::shared_ptr<RepresentationInterface> KieferSchuetzenbergerReduction::reduce(
        std::shared_ptr<RepresentationInterface> &waInstance) const {
    return reduce(waInstance, 300);
}

std::shared_ptr<RepresentationInterface> KieferSchuetzenbergerReduction::reduce(
        std::shared_ptr<RepresentationInterface> &waInstance, int K) const {
    std::shared_ptr<WeightedAutomatonInstance> A = std::dynamic_pointer_cast<WeightedAutomatonInstance>(waInstance);
    std::shared_ptr<WeightedAutomatonInstance> overLeftArrowA = backward_reduction(A, K);
    std::shared_ptr<WeightedAutomatonInstance> overLeftRightArrowA = forward_reduction(overLeftArrowA, K);
    return overLeftRightArrowA;
}

std::shared_ptr<WeightedAutomatonInstance> KieferSchuetzenbergerReduction::backward_reduction(
        std::shared_ptr<WeightedAutomatonInstance> &A, int K) const {
    auto rhoVectors = calculate_rho_backward_vectors(A, K);
    Eigen::MatrixXf rankTemp(A->get_states(), 1 + rhoVectors.size());

    rankTemp << *(A->get_eta());
    for (auto rhoVector : rhoVectors) {
        rankTemp << rhoVector;
    }
    Eigen::ColPivHouseholderQR<Eigen::Ref<Eigen::MatrixXf>> qr(rankTemp);
    int rank = qr.rank();

    Eigen::MatrixXf backwardBasis(A->get_states(), rank);
    backwardBasis << *(A->get_eta());
    for (int i = 0; i < rank - 1; i++) {
        backwardBasis << rhoVectors[i];
    }
    Eigen::RowVectorXf alphaArrow = *(A->get_alpha()) * backwardBasis;
    Eigen::VectorXf etaArrow = Eigen::MatrixXf::Zero(rank, 1);
    etaArrow(0,0) = 1;
    std::vector<std::shared_ptr<Eigen::MatrixXf>> muArrow = {};


    for (auto muX : A->get_mu()) {
        // x*A = b <=> A.transpose() * z = b.transpose(); x = z.transpose()
        // => x = (housholder(A.transpose()).solve(b.transpose())).transpose()
        Eigen::ColPivHouseholderQR<Eigen::MatrixXf> housholderX((*(muX) * backwardBasis).transpose());
        Eigen::MatrixXf muXArrow = (housholderX.solve(backwardBasis.transpose())).transpose();
        muArrow.push_back(std::make_shared<Eigen::MatrixXf>(muXArrow));
    }
    return std::make_shared<WeightedAutomatonInstance>(rank, A->get_number_input_characters(), alphaArrow, muArrow,
            etaArrow);
}

std::shared_ptr<WeightedAutomatonInstance> KieferSchuetzenbergerReduction::forward_reduction(
        std::shared_ptr<WeightedAutomatonInstance> &A, int K) const {
    auto rhoVectors = calculate_rho_forward_vectors(A, K);
    // FIXME continue

}

std::vector<Eigen::VectorXf>
KieferSchuetzenbergerReduction::calculate_rho_backward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                               int K) const {
    auto randomVectors = generate_random_vectors(A, K);
    auto sigmaK = generate_words_backwards(A, A->get_states());
    std::vector<Eigen::VectorXf> result = {};

    for (auto randomVector : randomVectors) {
        Eigen::VectorXf vI = Eigen::MatrixXf::Zero(A->get_states(), 1);
        for (auto word : sigmaK) {
             vI = vI + std::get<0>(word) * get_word_factor(std::get<1>(word), randomVector);
        }
        result.push_back(vI.transpose());
    }
    return result;
}

std::vector<Eigen::RowVectorXf>
KieferSchuetzenbergerReduction::calculate_rho_forward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                               int K) const {
    auto randomVectors = generate_random_vectors(A, K);
    auto sigmaK = generate_words_forwards(A, A->get_states());
    std::vector<Eigen::RowVectorXf> result = {};

    for (auto randomVector : randomVectors) {
        Eigen::RowVectorXf vI = Eigen::MatrixXf::Zero(1, A->get_states());
        for (auto word : sigmaK) {
            vI = vI + std::get<0>(word) * get_word_factor(std::get<1>(word), randomVector);
        }
        result.push_back(vI);
    }
    return result;
}

std::vector<std::tuple<Eigen::VectorXf, std::vector<uint>>>
KieferSchuetzenbergerReduction::generate_words_backwards(std::shared_ptr<WeightedAutomatonInstance> &A,
                                               int k) const {
    std::vector<std::tuple<Eigen::VectorXf,std::vector<uint>>> result;
    std::vector<uint> temp;
    if (k == 1) {
        result = {};
        for (uint i = 0; i < A->get_mu().size(); i++) {
            Eigen::VectorXf resultVect = *(A->get_mu()[i]) * *(A->get_eta());
            if (resultVect.isZero() == 0) {
                result.push_back({resultVect, {i}});
            }
        }
    } else {
        result = generate_words_backwards(A, k - 1);
        for (auto &element : result) {
            for (uint i = 0; i < A->get_mu().size(); i++) {
                Eigen::VectorXf resultVect = *(A->get_mu()[i]) * std::get<0>(element);
                if (resultVect.isZero() == 0) {
                    temp = std::get<1>(element);
                    temp.push_back(i);
                    result.push_back({resultVect, temp});
                }
            }
        }
    }
    return result;
}

std::vector<std::tuple<Eigen::RowVectorXf, std::vector<uint>>>
KieferSchuetzenbergerReduction::generate_words_forwards(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                          int k) const {
    std::vector<std::tuple<Eigen::RowVectorXf,std::vector<uint>>> result;
    std::vector<uint> temp;
    if (k == 1) {
        result = {};
        for (uint i = 0; i < A->get_mu().size(); i++) {
            Eigen::RowVectorXf resultVect = *(A->get_alpha()) * *(A->get_mu()[i]);
            if (resultVect.isZero() == 0) {
                result.push_back({resultVect, {i}});
            }
        }
    } else {
        result = generate_words_forwards(A, k - 1);
        for (auto &element : result) {
            for (uint i = 0; i < A->get_mu().size(); i++) {
                Eigen::RowVectorXf resultVect = std::get<0>(element) * *(A->get_mu()[i]);
                if (resultVect.isZero() == 0) {
                    temp = std::get<1>(element);
                    temp.push_back(i);
                    result.push_back({resultVect, temp});
                }
            }
        }
    }
    return result;
}

std::vector<Eigen::MatrixXi>
KieferSchuetzenbergerReduction::generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &A, int K)
const {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> uniform(1, A->get_states() * K);
    std::vector<Eigen::MatrixXi> randV;

    for (int i = 0; i < A->get_states(); i++) {
        Eigen::MatrixXi vect;
        for (int j = 0; j < A->get_number_input_characters(); j++) {
            for (int k = 0; k < A->get_states(); k++) {
                vect(j, k) = uniform(rng);
            }
        }
    }
    return randV;
}

int KieferSchuetzenbergerReduction::get_word_factor(std::vector<uint> word, Eigen::MatrixXi randVector) const {
    int result = 0;
    for (uint i = 0; i < word.size(); i++) {
        result *= randVector(word[i], i);
    }
    return result;
}
