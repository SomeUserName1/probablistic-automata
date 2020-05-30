#include <catch2/catch.hpp>

#include "../model/weighted_automata/WeightedAutomatonInstance.h"

static std::shared_ptr<WeightedAutomatonInstance> gen_wa();
static std::vector<Eigen::MatrixXi> gen_fixed_rand_v();
static std::vector<Eigen::MatrixXi> gen_fixed_rand_v3();

SCENARIO("Random Vectors have correct shape") {
    GIVEN("An automaton and a number K") {
        WHEN("generating random vectors") {
            THEN("|states| random row vectors of length |characters| are generated") {

            }
        }
    }
}

SCENARIO("Subtraction Automaton is constructed") {
    GIVEN("Two automata A and B") {
        WHEN("A and B are the (exactly) same automaton") {
            THEN("The weight of all words is 0") {

            }
        }WHEN("A and B do not have the same number of states") {
            THEN("An exception is thrown") {

            }
        }WHEN("B is an empty automaton") {
            THEN("The subtraction automaton is exactly A") {

            }
        }WHEN("A and B have the same amount of states") {
            THEN("The subtraction automaton has 2n states , the initial and final vector are of length 2n and the "
                 "transition matrices is a 2n square matrix") {

            }
        }WHEN("A and B are valid automata") {
            THEN("the transition matrices are block matrices with lower left and upper right block matrix is the zero"
                 " matrix and the upper left and lower right matrices are equivalent to As and Bs transition matrices"
                 " for the specific character respectively") {

            }THEN("The initial vector is a row vector of both automatas intitial vectors horizontally concetenated with "
                  "the second "
                  "one beeing inverted in terms of signum") {

            }THEN("The final vector is a column vector of both automatas final vector vertically concatenated") {

            }THEN("The amount of states is the sum of the automatas states") {

            }THEN("the number of characters is the same as the number of characters in the input automata") {

            }
        }
    }
}

SCENARIO("The equivalence of automata is tested") {
    GIVEN("Two Automata A and B") {
        WHEN("A and B are the exact same automata") {
            auto wa1 = gen_wa();
            auto wa2 = gen_wa();
            THEN("They are equal") {
                REQUIRE(*wa1 == *wa2);
            }
        }WHEN("The automata have differing numbers of input characters") {
            THEN("They are not equal") {

            }
        }WHEN("The automata are different in structure but equivalent in semantics") {
            THEN("They are equivalent") {

            }
        }WHEN("The Automata differ in semantics") {
            THEN("They are not equivalent") {

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
