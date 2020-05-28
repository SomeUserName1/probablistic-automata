#include <catch2/catch.hpp>
#include <chrono>
#include <thread>

SCENARIO("The program can be called from the command line", "[CLI]") {
    GIVEN("No arguments") {
        WHEN("The program is executed") {
            int status = std::system("./ssm > test.txt");

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            status = std::system("ps aux | grep ssm | awk '{print $2}' | xargs -I{} kill -9 {}");

            std::stringstream buf;
            buf << std::ifstream("test.txt").rdbuf();

            THEN("The text user interface is launched") {
                REQUIRE(buf.str().starts_with("Hello"));
            }
        }
    }
    GIVEN("Incomplete arguments") {
        std::string task(" -t Reduce ");
        std::string model(" -m WA ");

        WHEN("the program is executed ") {
            int status = std::system("./ssm" + task + model "> test.txt");

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            status = std::system("ps aux | grep ssm | awk '{print $2}' | xargs -I{} kill -9 {}");

            std::stringstream buf;
            buf << std::ifstream("test.txt").rdbuf();

            THEN("The text user interface is launched") {
                REQUIRE(buf.str().starts_with("Hello"));
            }
        }
    }
    GIVEN("Text and gui switches") {
        WHEN("The program is called with the gui switch") {
            THEN("An exception is thrown as its not implemented") {

            }
        }
        WHEN("The program is called with the TUI switch") {
            THEN("The TUI is executed") {

            }
        }
    }
    GIVEN("All arguments") {
        WHEN("The arguments are correctly specified") {

            THEN("The program executes correctly") {

            }
        }
        WHEN("An incorrect task is specified") {
            THEN("An exception is thrown") {

            }
        }
        WHEN("An incorrect model type is specified") {
            THEN("An exception is thrown") {

            }
        }
        WHEN("A non-existing input path is specified") {
            THEN("An exception is thrown") {

            }
        }
        WHEN("A badly formatted input path is specified") {
            THEN("An exception is thrown") {

            }
        }
        WHEN("A badly formatted output path is specified") {
            THEN("An exception is thrown") {

            }
        }
    }
}