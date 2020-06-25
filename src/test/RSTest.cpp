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
    std::string input = "AbbBcDDe + ABC AbBa";
    WHEN("parsing the input") {
      THEN("it parses") {
        std::string name;
        name = extract_entity_name(input);
        while (!name.empty()) {
          std::cout << name << std::endl;
          name = extract_entity_name(input);
        }
        REQUIRE(true);
      }
    }
  }
}
