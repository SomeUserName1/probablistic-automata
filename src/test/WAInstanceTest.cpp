#include <catch2/catch.hpp>

#include "../model/weighted_automata/WeightedAutomatonInstance.h"

static std::shared_ptr<WeightedAutomatonInstance> gen_wa();
// static std::vector<Eigen::MatrixXi> gen_fixed_rand_v();
// static std::vector<Eigen::MatrixXi> gen_fixed_rand_v3();
static bool double_compare(double x, double y);

SCENARIO("Random Vectors have correct shape") {
    GIVEN("An automaton and a number K") {
        auto mu = {std::make_shared<Eigen::MatrixXd>(10, 10)};
        auto automaton = std::make_shared<WeightedAutomatonInstance>(10, 4, std::make_shared<Eigen::RowVectorXd>(10),
                                                                     mu,
                                                                     std::make_shared<Eigen::VectorXd>(10));
        uint K = 100;
        WHEN("generating random vectors") {
            auto vectors = WeightedAutomatonInstance::generate_random_vectors(automaton, K);
            THEN("|states| random row vectors of length |characters| are generated") {
                REQUIRE(vectors.size() == 10);
                for (const auto &vector : vectors) {
                    REQUIRE(vector.rows() == 1);
                    REQUIRE(vector.cols() == 4);
                }
            }
        }
    }
}

SCENARIO("Subtraction Automaton is constructed") {
    GIVEN("Two automata A and B") {
        auto wa1 = gen_wa();
        std::shared_ptr<WeightedAutomatonInstance> wa2;
        WHEN("A and B are the (exactly) same automaton") {
            wa2 = gen_wa();
            auto subtractionAutomaton = WeightedAutomatonInstance::create_subtraction_automaton(*wa1, *wa2);
            // TODO powerset of words up to k as function
            std::vector<std::vector<uint>> words = {{0}, {1}, {0,0}, {0,1}, {1,0}, {1,1}, {0,0,0}, {0,0,1}, {0,1,0},
                                                    {0,1,1}, {1,0,0}, {1,0,1}, {1,1,0}, {1,1,1}};
            THEN("The weight of example words is 0 in the subtraction automaton") {
                for (const auto& word : words) {
                    REQUIRE(double_compare(subtractionAutomaton->process_word(word), 0.0));
                }
            }
        }
        WHEN("A and B do not have the same number of input characters") {
            int states = 4;
            int characters = 3;
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
            auto mu3 = std::make_shared<Eigen::MatrixXd>(states, states);
            *mu3 << 0, 1, 1, 0,
                    0, 0, 0, 1,
                    0, 0, 0, 1,
                    0, 0, 0, 0;
            std::vector<std::shared_ptr<Eigen::MatrixXd>> mu = {mu1, mu2};
            wa2 = std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
            auto subtractionAutomaton = WeightedAutomatonInstance::create_subtraction_automaton(*wa1, *wa2);
            std::vector<uint> word = {0, 2, 2, 2};
            THEN("The subtraction automaton is non-zero") {
                REQUIRE(double_compare(subtractionAutomaton->process_word(word), 0.0));
            }
        }
        WHEN("B is an empty automaton") {
            int states = 0;
            int characters = 0;
            auto alpha = std::make_shared<Eigen::RowVectorXd>(states);
            auto eta = std::make_shared<Eigen::VectorXd>(states);
            auto mu1 = std::make_shared<Eigen::MatrixXd>(states, states);
            auto mu2 = std::make_shared<Eigen::MatrixXd>(states, states);
            auto mu3 = std::make_shared<Eigen::MatrixXd>(states, states);
            std::vector<std::shared_ptr<Eigen::MatrixXd>> mu = {mu1, mu2};
            wa2 = std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
            auto subtractionAutomaton = WeightedAutomatonInstance::create_subtraction_automaton(*wa1, *wa2);
            std::vector<std::vector<uint>> words = {{0,1}, {0,0}};
            THEN("The subtraction automaton is exactly A (checked by probing two words)") {
                REQUIRE(double_compare(subtractionAutomaton->process_word(words[0]), wa1->process_word(words[0])));
                REQUIRE(double_compare(subtractionAutomaton->process_word(words[1]), wa1->process_word(words[1])));
            }
        }
    }
}

SCENARIO("The equivalence of automata is tested") {
    GIVEN("Two Automata A and B") {
        auto wa1 = gen_wa();
        std::shared_ptr<WeightedAutomatonInstance> wa2;
        WHEN("A and B are the exact same automata") {
            wa2 = gen_wa();
            THEN("They are equal") {
                REQUIRE(*wa1 == *wa2);
            }
        }WHEN("The automata have differing numbers of input characters") {
            int states = 4;
            int characters = 3;
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
            auto mu3 = std::make_shared<Eigen::MatrixXd>(states, states);
            *mu3 << 0, 0, 0, 0,
                    0, 0, 0, 1,
                    0, 0, 0, 1,
                    0, 0, 0, 0;
            std::vector<std::shared_ptr<Eigen::MatrixXd>> mu = {mu1, mu2};
            wa2 = std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);

            THEN("They are not equal") {
                REQUIRE(*wa1 != *wa2);
            }
        }WHEN("The automata are different in structure but equivalent in semantics") {
            int states = 3;
            int characters = 2;
            auto alpha = std::make_shared<Eigen::RowVectorXd>(states);
            *alpha << 2, 0, 0;
            auto eta = std::make_shared<Eigen::VectorXd>(states);
            *eta << 0, 0, 1;
            auto mu1 = std::make_shared<Eigen::MatrixXd>(states, states);
            *mu1 << 0, 1, 0,
                    0, 0, 0,
                    0, 0, 0;
            auto mu2 = std::make_shared<Eigen::MatrixXd>(states, states);
            *mu2 << 0, 0, 0,
                    0, 0, 1,
                    0, 0, 0;
            std::vector<std::shared_ptr<Eigen::MatrixXd>> mu = {mu1, mu2};
            wa2 = std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
            THEN("They are equivalent") {
                REQUIRE(*wa1 == *wa2);
            }
        }WHEN("The Automata differ in semantics") {
            int states = 3;
            int characters = 2;
            auto alpha = std::make_shared<Eigen::RowVectorXd>(states);
            *alpha << 2, 0, 0;
            auto eta = std::make_shared<Eigen::VectorXd>(states);
            *eta << 0, 0, 1;
            auto mu1 = std::make_shared<Eigen::MatrixXd>(states, states);
            *mu1 << 1, 1, 0,
                    0, 0, 0,
                    0, 0, 0;
            auto mu2 = std::make_shared<Eigen::MatrixXd>(states, states);
            *mu2 << 0, 0, 0,
                    0, 0, 1,
                    0, 0, 0;
            std::vector<std::shared_ptr<Eigen::MatrixXd>> mu = {mu1, mu2};
            wa2 = std::make_shared<WeightedAutomatonInstance>(states, characters, alpha, mu, eta);
            THEN("They are not equivalent") {
                    REQUIRE(*wa1 != *wa2);
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

/*std::vector<Eigen::MatrixXi> gen_fixed_rand_v() {
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
}*/

bool double_compare(double x, double y) {
    double maxXYOne = std::max( { 1.0, std::fabs(x) , std::fabs(y) } ) ;

    return std::fabs(x - y) <= std::numeric_limits<double>::epsilon()*maxXYOne ;
}
