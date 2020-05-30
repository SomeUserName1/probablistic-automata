#include <catch2/catch.hpp>

SCENARIO("The input can be specified over the tui", "[TUI]") {
    GIVEN("The task is to be selected") {
        WHEN("A valid task number is entered") {
            THEN("The correct task is selected") {

            }
        }WHEN("An invalid task number is entered") {
            THEN("The user is informed and probed again") {

            }
        }WHEN("A not implemented task is selected") {
            THEN("A NotImplementedException is thrown") {

            }
        }
    }

    GIVEN("The model is to be selected") {
        WHEN("A valid model number is entered") {
            THEN("The correct model is selected") {

            }
        }WHEN("An invalid model number is entered") {
            THEN("The user is informed an probed again") {

            }
        }
    }

    GIVEN("The input or output method is to be selected") {
        WHEN("A valid IO method number is entered") {
            THEN("The correct IO Method is selected") {

            }
        }WHEN("An invalid number is entered") {
            THEN("The user is informed and reprobed") {

            }
        }
    }

    GIVEN("File input is chosen") {
        WHEN("A valid file path is specified") {
            THEN("The file is read into a string") {

            }
        }WHEN("An invalid file path is entered") {
            THEN("The user is informed and probed again") {

            }
        }
    }

    GIVEN("Stdin is chosen as input method") {
        WHEN("specifying the input") {
            THEN("the input can be terminated by two consecutive newlines") {

            }THEN("The input can be terminated by STRG+D/EOF") {

            }THEN("What the user entered is read into a string") {

            }
        }
    }

    GIVEN("File output is chosen") {
        WHEN("A valid file path is specified") {
            THEN("The output is written to the file") {

            }
        }WHEN("An invalid file path is entered") {
            THEN("The user is informed and probed again") {

            }
        }
    }

    GIVEN("A reduction method is to be chosen") {
        WHEN("A valid reduction method number is entered") {
            THEN("The correct reduction Method is selected") {

            }
        }WHEN("An invalid number is entered") {
            THEN("The user is informed and reprobed") {

            }
        }
    }
}
