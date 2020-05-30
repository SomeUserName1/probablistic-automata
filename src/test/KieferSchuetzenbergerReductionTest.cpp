#include <catch2/catch.hpp>
#include <sstream>
#include <vector>
#include <iostream>

#include "../model/weighted_automata/KieferSchuetzenbergerReduction.h"

static std::shared_ptr<WeightedAutomatonInstance> gen_wa();

static std::vector<Eigen::MatrixXi> gen_fixed_rand_v();

static std::vector<Eigen::MatrixXi> gen_fixed_rand_v3();

SCENARIO("The word factors are calculated correctly as specified in the paper") {
    GIVEN("Fixed random vectors") {
        auto mat = gen_fixed_rand_v();
        WHEN("Calculating the word factor") {
            int factor1 = KieferSchuetzenbergerReduction::get_word_factor({0}, mat[0]);
            int factor2 = KieferSchuetzenbergerReduction::get_word_factor({0, 1}, mat[0]);

            int factor3 = KieferSchuetzenbergerReduction::get_word_factor({0}, mat[1]);
            int factor4 = KieferSchuetzenbergerReduction::get_word_factor({0, 1}, mat[1]);

            int factor5 = KieferSchuetzenbergerReduction::get_word_factor({0}, mat[2]);
            int factor6 = KieferSchuetzenbergerReduction::get_word_factor({0, 1}, mat[2]);

            int factor7 = KieferSchuetzenbergerReduction::get_word_factor({0}, mat[3]);
            int factor8 = KieferSchuetzenbergerReduction::get_word_factor({0, 1}, mat[3]);

            THEN("Is the same as when computed by hand") {
                REQUIRE(factor1 == 9);
                REQUIRE(factor2 == 99);

                REQUIRE(factor3 == 2);
                REQUIRE(factor4 == 6);

                REQUIRE(factor5 == 2);
                REQUIRE(factor6 == 22);

                REQUIRE(factor7 == 4);
                REQUIRE(factor8 == 36);
            }
        }
    }
}
SCENARIO("The random vectors have the correct shape") {
    GIVEN("An automaton A and a number K") {
        auto mu = {std::make_shared<Eigen::MatrixXd>(10, 10)};
        auto automaton = std::make_shared<WeightedAutomatonInstance>(10, 4, std::make_shared<Eigen::RowVectorXd>(10),
                                                                     mu,
                                                                     std::make_shared<Eigen::VectorXd>(10));
        int K = 100;
        WHEN("generating random vectors") {
            auto vectors = KieferSchuetzenbergerReduction::generate_random_vectors(automaton, K);
            THEN("yields |states| random matrices of shape |characters| * |states| are generated") {
                REQUIRE(vectors.size() == 10);
                for (const auto &vector : vectors) {
                    REQUIRE(vector.rows() == 4);
                    REQUIRE(vector.cols() == 10);
                }
            }
        }
    }
}
SCENARIO("The generating words yields only valid words") {
    GIVEN("An automaton A") {
        auto A = gen_wa();

        WHEN("Generating words forwards") {
            auto wordsForward = KieferSchuetzenbergerReduction::generate_words_forwards(A, A->get_states());
            THEN("The generated words and their vectors are the same as when calculated by hand") {
                REQUIRE(wordsForward.size() == 2);

                auto[vector, word] = wordsForward[0];
                REQUIRE(word.size() == 1);
                REQUIRE(word[0] == 0);
                REQUIRE(vector->rows() == 1);
                REQUIRE(vector->cols() == A->get_states());
                auto hcVector = Eigen::RowVectorXd(A->get_states());
                hcVector << 0, 1, 1, 0;
                REQUIRE((*vector - hcVector).norm() == 0);

                std::tie(vector, word) = wordsForward[1];
                REQUIRE(word.size() == 2);
                REQUIRE(word[0] == 0);
                REQUIRE(word[1] == 1);
                REQUIRE(vector->rows() == 1);
                REQUIRE(vector->cols() == A->get_states());
                hcVector << 0, 0, 0, 2;
                REQUIRE((*vector - hcVector).norm() == 0);
            }
        }
        WHEN("Generating words backwards") {
            auto wordsBackward = KieferSchuetzenbergerReduction::generate_words_backwards(A, A->get_states());
            THEN("The generated words and their vectors are the same as when calculated by hand") {
                REQUIRE(wordsBackward.size() == 2);

                auto[vector, word] = wordsBackward[0];
                REQUIRE(word.size() == 1);
                REQUIRE(word[0] == 1);
                REQUIRE(vector->rows() == A->get_states());
                REQUIRE(vector->cols() == 1);
                auto hcVector = Eigen::VectorXd(A->get_states());
                hcVector << 0, 1, 1, 0;
                REQUIRE((*vector - hcVector).norm() == 0);

                std::tie(vector, word) = wordsBackward[1];
                REQUIRE(word.size() == 2);
                REQUIRE(word[0] == 0);
                REQUIRE(word[1] == 1);
                REQUIRE(vector->rows() == A->get_states());
                REQUIRE(vector->cols() == 1);
                hcVector << 2, 0, 0, 0;
                REQUIRE((*vector - hcVector).norm() == 0);
            }
        }
    }
}
SCENARIO("The rho vectors are calculated correctly as specified in the paper") {
    GIVEN("An automaton A and fixed random vectors R") {
        auto A = gen_wa();
        auto randomVectors = gen_fixed_rand_v();
        WHEN("calculating the rho vectors forward") {
            auto rhoForward = KieferSchuetzenbergerReduction::calculate_rho_forward_vectors(A, randomVectors);
            THEN("They are the same as when calculating them by hand") {
                Eigen::RowVectorXd v1(4);
                Eigen::RowVectorXd v2(4);
                Eigen::RowVectorXd v3(4);
                Eigen::RowVectorXd v4(4);
                v1 << 0, 9, 9, 198;
                v2 << 0, 2, 2, 12;
                v3 << 0, 2, 2, 44;
                v4 << 0, 4, 4, 72;
                std::vector<Eigen::MatrixXd> vect = {v1, v2, v3, v4};
                for (int i = 0; i < A->get_states(); i++) {
                    REQUIRE((vect[i] - rhoForward[i]).norm() == 0);
                }
            }
        }
        WHEN("calculating the rho vectors backward") {
            auto rhoBackward = KieferSchuetzenbergerReduction::calculate_rho_backward_vectors(A, randomVectors);
            THEN("They are the same as when calculating them by hand") {
                Eigen::VectorXd v1(4);
                Eigen::VectorXd v2(4);
                Eigen::VectorXd v3(4);
                Eigen::VectorXd v4(4);
                v1 << 198, 6, 6, 0;
                v2 << 12, 12, 12, 0;
                v3 << 44, 1, 1, 0;
                v4 << 72, 5, 5, 0;
                std::vector<Eigen::MatrixXd> vect = {v1, v2, v3, v4};
                for (int i = 0; i < A->get_states(); i++) {
                    REQUIRE((vect[i] - rhoBackward[i]).norm() == 0);
                }
            }
        }
    }
}
SCENARIO("The forward and backward reductions are calculated correctly as specified in the paper") {
    GIVEN("An automaton A and fixed random vectors R") {
        auto wa = gen_wa();
        auto randV = gen_fixed_rand_v();
        WHEN("calculating the forward reduction") {
            auto minWA = KieferSchuetzenbergerReduction::forward_reduction(wa, randV);
            THEN("They are the same as when calculating them by hand") {
                REQUIRE(minWA->get_states() == 3);
                REQUIRE(minWA->get_number_input_characters());

                auto trueAlpha = Eigen::RowVectorXd(3);
                trueAlpha << 1, 0, 0;
                REQUIRE((*(minWA->get_alpha()) - trueAlpha).norm() == 0);

                auto trueMu1 = Eigen::MatrixXd(3, 3);
                auto trueMu2 = Eigen::MatrixXd(3, 3);
                trueMu1 << 0, -1.0 / 24.0, 33.0 / 48.0, 0, 0, 0, 0, 0, 0;
                trueMu2 << 0, 0, 0, 0, 1.0 / 8.0, -9.0 / 16.0, 0, 1.0 / 36.0, -1.0 / 8.0;
                std::vector<Eigen::MatrixXd> trueMu = {trueMu1, trueMu2};
                for (int i = 0; i < 2; i++) {
                    REQUIRE((minWA->get_mu()[i])->isApprox(trueMu[i]));
                }

                auto trueEta = Eigen::VectorXd(3);
                trueEta << 0, 198, 12;
                REQUIRE((*(minWA->get_eta()) - trueEta).norm() == 0);
            }
        }
        WHEN("calculating the backward reduction") {
            auto minWA = KieferSchuetzenbergerReduction::backward_reduction(wa, randV);
            THEN("They are the same as when calculating them by hand") {
                REQUIRE(minWA->get_states() == 3);
                REQUIRE(minWA->get_number_input_characters());

                auto trueAlpha = Eigen::RowVectorXd(3);
                trueAlpha << 0, 198, 12;
                REQUIRE((*(minWA->get_alpha()) - trueAlpha).norm() == 0);

                auto trueMu1 = Eigen::MatrixXd(3, 3);
                auto trueMu2 = Eigen::MatrixXd(3, 3);
                trueMu1 << 0, 0, 0, 0, 1.0 / 16.0, 1.0 / 8.0, 0, -1.0 / 32.0, -1.0 / 16.0;
                trueMu2 << 0, 0, 0, -1.0 / 192.0, 0, 0, 11.0 / 128.0, 0, 0;
                std::vector<Eigen::MatrixXd> trueMu = {trueMu1, trueMu2};
                for (int i = 0; i < 2; i++) {
                    REQUIRE((minWA->get_mu()[i])->isApprox(trueMu[i]));
                }

                auto trueEta = Eigen::VectorXd(3);
                trueEta << 1, 0, 0;
                REQUIRE((*(minWA->get_eta()) - trueEta).norm() == 0);
            }
        }
    }
}

SCENARIO("When executing the full reduction") {
    GIVEN("an initial automaton and the random vectors") {
        auto A = gen_wa();
        auto randomVectors4 = gen_fixed_rand_v();
        auto randomVectors3 = gen_fixed_rand_v3();
        WHEN("Executing the exact same steps as when calling reduce on an WA") {
            std::shared_ptr<WeightedAutomatonInstance> minWA = KieferSchuetzenbergerReduction::forward_reduction(A,
                randomVectors4);
            minWA = KieferSchuetzenbergerReduction::backward_reduction(minWA, randomVectors3);
            THEN("The result is equivalent to what is calculated by hand") {
                REQUIRE(minWA->get_states() == 3);
                REQUIRE(minWA->get_number_input_characters());

                auto trueAlpha = Eigen::RowVectorXd(3);
                trueAlpha << 0, 10, 128;
                REQUIRE((minWA->get_alpha())->isApprox(trueAlpha));

                auto trueMu1 = Eigen::MatrixXd(3, 3);
                auto trueMu2 = Eigen::MatrixXd(3, 3);
                trueMu1 << (double)5.256737805990320e-20, (double)-1.612066260503700e-18, (double)-2.821115955881475e-18,
                           (double)2.775557561562891e-17, (double)-28.0/221.0, (double)-49.0/221.0,
                           0,  (double)16.0/221.0,  (double)28.0/221.0;
                trueMu2 << (double)2.523234146875356e-18, (double)9.617563887311330e-17, (double)1.193872527811887e-15,
                           (double)64.0/221.0,  (double)2.775557561562891e-17, (double)6.661338147750939e-16,
                           (double)-5.0/221.0,  (double)-3.469446951953614e-18, (double)-5.551115123125783e-17;
                std::vector<Eigen::MatrixXd> trueMu = {trueMu1, trueMu2};
                for (int i = 0; i < 2; i++) {
                    REQUIRE((minWA->get_mu()[i])->isApprox(trueMu[i]));
                }

                auto trueEta = Eigen::VectorXd(3);
                trueEta << 1, 0, 0;
                REQUIRE((minWA->get_eta())->isApprox(trueEta));
            }
        }
    }
}

std::shared_ptr<WeightedAutomatonInstance> gen_wa() {
    int states = 4;
    int characters = 2;
    auto alpha = std::make_shared<Eigen::RowVectorXd>(states);
    *alpha << 1, 0, 0, 0;
    auto eta = std::make_shared<Eigen::VectorXd>(states);
    *eta << 0, 0, 0, 1;
    auto mu1 = std::make_shared<Eigen::MatrixXd>(states, states);
    *mu1 << 0, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0;
    auto mu2 = std::make_shared<Eigen::MatrixXd>(states, states);
    *mu2 << 0, 0, 0, 0,
            0, 0, 0, 1,
            0, 0, 0, 1,
            0, 0, 0, 0;
    std::vector<std::shared_ptr<Eigen::MatrixXd>> mu = {mu1, mu2};
    return std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
}

std::vector<Eigen::MatrixXi> gen_fixed_rand_v() {
    Eigen::MatrixXi mat1(2, 4);
    Eigen::MatrixXi mat2(2, 4);
    Eigen::MatrixXi mat3(2, 4);
    Eigen::MatrixXi mat4(2, 4);
    mat1 << 9, 5, 5, 7, 6, 11, 2, 1;
    mat2 << 2, 3, 1, 2, 12, 3, 9, 4;
    mat3 << 2, 7, 9, 10, 1, 11, 2, 6;
    mat4 << 4, 5, 2, 10, 5, 9, 5, 5;
    return {mat1, mat2, mat3, mat4};
}

std::vector<Eigen::MatrixXi> gen_fixed_rand_v3() {
    Eigen::MatrixXi mat1(2, 3);
    Eigen::MatrixXi mat2(2, 3);
    Eigen::MatrixXi mat3(2, 3);
    mat1 << 1, 6, 2, 4, 5, 4;
    mat2 << 8, 1, 8, 7, 8, 2;
    mat3 << 1, 6, 5, 9, 2, 1;
    return {mat1, mat2, mat3};
}
