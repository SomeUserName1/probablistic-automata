#include <catch2/catch.hpp>

#include "../models/rewrite_systems/RewriteSystemModel.h"
#include "../ui/UserInterface.h"
#include "../util/ParseUtils.h"

SCENARIO("Parsing rules with many multiplicities works") {
  GIVEN("A rule with lots of multiplicities in the species") {
    std::string input = "2 Au + 2B -> Au2B2 	, 3.0";
    auto model = std::make_shared<RewriteSystemModel>();
    WHEN("Parsing the rule") {
      auto ri = model->parse(input);
      auto rs = std::static_pointer_cast<RewriteSystem>(ri);
      THEN("it's parsed correctly") {
        std::vector<std::string> trueMap = {"Au", "B", "Au2B2"};

        auto lhs1 = std::make_shared<RewriteSystem::Term>(2, 0);
        auto lhs2 = std::make_shared<RewriteSystem::Term>(2, 1);
        auto rhs1 = std::make_shared<RewriteSystem::Term>(1, 2);

        std::vector<std::shared_ptr<RewriteSystem::Term>> lhs = {lhs1, lhs2};
        std::vector<std::shared_ptr<RewriteSystem::Term>> rhs = {rhs1};
        std::vector<std::shared_ptr<RewriteSystem::Rule>> trueRule = {
            std::make_shared<RewriteSystem::Rule>(3.0, lhs, rhs)};

        std::cout << rs->pretty_print() << std::endl;
        for (size_t i = 0; i < trueMap.size(); i++) {

          REQUIRE(rs->get_mapping()[i] == trueMap[i]);
        }
        REQUIRE(rs->get_rules()[0]->equivalent((trueRule[0])));
      }
    }
  }
}

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
                        "  ApB    -> Ap + B \t, 4.0\n"
                        "  B";
    std::vector<std::string> result = {"Au", "Ap", "Ap", "Au", "Au", "B",  "Au",
                                       "B",  "Au", "B",  "Au", "B",  "Ap", "B",
                                       "Ap", "B",  "Ap", "B",  "Ap", "B",  "B"};
    WHEN("parsing the input") {
      THEN("it parses") {
        std::string name;
        for (size_t i = 0; i < result.size(); i++) {
          name = extract_atomic_name(input);
          REQUIRE(result[i] == name);
        }
      }
    }
  }
}
