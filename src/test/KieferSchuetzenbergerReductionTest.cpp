#include <catch2/catch.hpp>

SCENARIO("Test the partial functions") {
    GIVEN("Fixed random vectors") {
        WHEN("Calculating the word factor") {
            THEN("Is the same as when computed by hand") {

            }
        }
    }

    GIVEN("An automaton A and a number K") {
        WHEN("generating random vectors") {
            THEN("yields |states| * |characters| random row vectors of length |states| are generated") {

            }
        }
    }

    GIVEN("An automaton A") {
        WHEN("Generating words forwards") {
            THEN("The generated words and their vectors are the same as when calculated by hand") {

            }
        }
        WHEN("Generating words backwards") {
            THEN("The generated words and their vectors are the same as when calculated by hand") {

            }
        }
    }

    GIVEN("An automaton A and fixed random vectors R") {
        WHEN("calculating the rho vectors forward") {
            THEN("They are the same as when calculating them by hand") {

            }
        }
        WHEN("calculating the rho vectors backward") {
            THEN("They are the same as when calculating them by hand") {

            }
        }
    }

    GIVEN("An automaton A and fixed random vectors R") {
        WHEN("calculating the forward reduction") {
            THEN("They are the same as when calculating them by hand") {

            }
        }
        WHEN("calculating the backward reduction") {
            THEN("They are the same as when calculating them by hand") {

            }
        }
    }
}
SCENARIO("Executing the reduction") {
    GIVEN("An Automaton A") {
        WHEN("Reducing A") {
            THEN("the reduction yields the same as doing it by hand")
        }
    }
    GIVEN("An automaton A and a number K") {
        WHEN("Reducting with A as input and K as paramater") {
            THEN("The result is correct with Pr 2*(1 - n/K)") {

            }
        }
    }
}