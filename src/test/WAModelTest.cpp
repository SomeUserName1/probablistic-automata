#include <catch2/catch.hpp>

SCENARIO("Testing static variables") {
    WHEN("The name is probed") {
        THEN("'Weighted Automaton Model' is returned") {

        }
    }WHEN("The reduction methods are probed") {
        THEN("The available methods are listed (i.e. now only KieferSchuetzenbergerReduction)") {

        }
    }WHEN("The conversion methods are probed") {
        THEN("The available methods are listed (currenlty none)") {

        }
    }
}

SCENARIO("Parsing input strings to WA Instances") {
    GIVEN("A valid WA") {
        WHEN("There is whitespace in the input") {
            THEN("It does not interfere with the syntax and semantics of the WA") {

            }
        }WHEN("It is parsed") {
            THEN("A valid WA instance is returned") {

            }
        }
    }GIVEN("An empty or underspecified automaton") {
        WHEN("Parsing it") {
            THEN("The user is informed that something is missing and what exactly is missing") {

            }
        }
    }GIVEN("An automaton in the wrong order (i.e. one tuple element is specified before an earlier one") {
        WHEN("Parsing it") {
            THEN("The user is informed that the tuple is not ordered an which element would have to be specified "
                 "next") {

            }
        }
    }GIVEN("A fully specified automaton in the correct order but with otherwise incorrect information") {
        WHEN("The initial vector has more or less elements than there are states") {
            THEN("An invalid argument exception with helpful error message is thrown") {

            }
        }WHEN("There are more or less transition matrices than characters") {
            THEN("An invalid argument exception with helpful error message is thrown") {

            }
        }WHEN("The final vector has more or less elements than there are states") {
            THEN("An invalid argument exception with helpful error message is thrown") {

            }
        }
    }
}
