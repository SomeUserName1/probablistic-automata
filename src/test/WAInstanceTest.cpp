#include <catch2/catch.hpp>

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
        }
        WHEN("A and B do not have the same number of states") {
            THEN("An exception is thrown") {

            }
        }
        WHEN("B is an empty automaton") {
            THEN("The subtraction automaton is exactly A") {

            }
        }
        WHEN("A and B have the same amount of states") {
            THEN("The subtraction automaton has 2n states , the initial and final vector are of length 2n and the "
                 "transition matrices is a 2n square matrix") {

            }
        }
        WHEN("A and B are valid automata") {
            THEN("the transition matrices are block matrices with lower left and upper right block matrix is the zero"
                 " matrix and the upper left and lower right matrices are equivalent to As and Bs transition matrices"
                 " for the specific character respectively") {

            }
            THEN("The initial vector is a row vector of both automatas intitial vectors horizontally concetenated with "
                 "the second "
                 "one beeing inverted in terms of signum") {

            }
            THEN("The final vector is a column vector of both automatas final vector vertically concatenated") {

            }
            THEN("The amount of states is the sum of the automatas states") {

            }
            THEN("the number of characters is the same as the number of characters in the input automata") {

            }
        }
    }
}

SCENARIO("The equivalence of automata is tested") {
    GIVEN("Two Automata A and B") {
        WHEN("A and B are the exact same automata") {
            THEN("They are equal") {

            }
        }
        WHEN("The automata have differing numbers of input characters") {
            THEN("They are not equal") {

            }
        }
        WHEN("The automata are different in structure but equivalent in semantics") {
            THEN("They are equivalent") {

            }
        }
        WHEN("The Automata differ in semantics") {
            THEN("They are not equivalent") {

            }
        }
    }
}