#include <catch2/catch.hpp>

#include "../ui/UserInterface.h"
#include "../model/rewrite_systems/RewriteSystemModel.h"
#include "../util/ParseUtils.h"

SCENARIO("Test Parsing") {
  GIVEN("Input") {
    std::string input =
        UserInterface::read_file("../src/test/stoichometric_input.txt");
    auto model = std::make_shared<RewriteSystemModel>();
    WHEN("parsing the input") {
      model->parse(input);
      THEN("it parses") { REQUIRE(true); }
    }
  }

  GIVEN("Input") {
    std::string input = "Au     -> Ap\t \t, 1.0\n"
                        "  Ap     -> Au\t \t, 2.0\n"
                        "  Au + B -> AuB\t \t, 3.0\n"
                        "  AuB    -> Au + B\t, 4.0\n"
                        "  Ap + B -> ApB\t \t, 3.0\n"
                        "  ApB    -> Ap + B \t, 4.0";
    std::vector<std::string> result = {"Au", "Ap", "Ap", "Au", "Au", "B", "Au", "B",
                                       "Au", "B", "Au", "B", "Ap", "B", "Ap", "B",
                                       "Ap", "B", "Ap", "B"};
    WHEN("parsing the input") {
      THEN("it parses") {
        std::string name;
        name = extract_entity_name(input);
        size_t i = 0;
        while (!name.empty()) {
          REQUIRE(result[i] == name);
          i++;
          name = extract_entity_name(input);
        }
        REQUIRE(true);
      }
    }
  }
}
