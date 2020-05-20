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
    auto randomVectors = generate_random_vectors(A, K);
    auto rhoVectors = calculate_rho_backward_vectors(A, randomVectors);
}

std::shared_ptr<WeightedAutomatonInstance> KieferSchuetzenbergerReduction::forward_reduction(
        std::shared_ptr<WeightedAutomatonInstance> &A, int K) const {
    auto randomVectors = generate_random_vectors(A, K);
    auto rhoVectors = calculate_rho_forward_vectors(A, randomVectors);

}

std::vector<Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>>
        KieferSchuetzenbergerReduction::generate_random_vectors(std::shared_ptr<WeightedAutomatonInstance> &A, int K)
        const {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> uniform(1, A->get_states() * K);

    std::vector<Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>> randV;

    for (int i = 0; i < A->get_states(); i++) {
        Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> vect;
        for (int j = 0; j < A->get_number_input_characters(); j++) {
            for (int k = 0; k < A->get_states(); k++) {
                vect(j, k) = uniform(rng);
            }
        }
    }
    return randV;
}

std::vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>
KieferSchuetzenbergerReduction::calculate_rho_backward_vectors(std::shared_ptr<WeightedAutomatonInstance> &A,
                                                               std::vector<Eigen::Matrix<int, Eigen::Dynamic,
                                                               Eigen::Dynamic>> randVectors) const {
   // generate Sigma^states eta = { mu_i Sigma^states-1 eta }
   auto sigmaK = generate_words(A, A->get_states());
   std::vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>> result = {};

   for (auto randomVector : randVectors) {
       Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> vI = Eigen::MatrixXf::Zero();
       for (auto word : sigmaK) {
            vI = vI + std::get<0>(word) * get_factor();
       }
   }
}

std::vector<std::tuple<Eigen::Matrix<float, Eigen::Dynamic, 1>,std::vector<uint>>>
KieferSchuetzenbergerReduction::generate_words(std::shared_ptr<WeightedAutomatonInstance> &A,
                                               int k) const {
    std::vector<std::tuple<Eigen::Matrix<float, Eigen::Dynamic, 1>,std::vector<uint>>> result;
    if (k == 1) {
        result = {};
        for (uint i = 0; i < A->get_mu().size(); i++) {
            Eigen::Matrix<float, Eigen::Dynamic, 1> resultVect = *(A->get_mu()[i]) * *(A->get_eta());
            if (resultVect.isZero() == 0) {
                result.push_back({resultVect, {i}});
            }
        }
    } else {
        result = generate_words(A, k - 1);
        for (auto &element : result) {
            for (uint i = 0; i < A->get_mu().size(); i++) {
                Eigen::Matrix<float, Eigen::Dynamic, 1> resultVect = *(A->get_mu()[i]) * std::get<0>(element);
                if (resultVect.isZero() == 0) {
                    result.push_back({resultVect, std::get<1>(element).push_back(i)});
                }
            }
        }
    }
    return result;
}
