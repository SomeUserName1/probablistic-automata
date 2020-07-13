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
        std::vector<std::string> trueMap = {"Au", "B"};
        std::vector<std::vector<unsigned int>> trueSpecies = {
            {1, 0}, {0, 1}, {2, 2}};

        std::array<unsigned int, 2> lhs1 = {2, 0};
        std::array<unsigned int, 2> lhs2 = {2, 1};
        std::array<unsigned int, 2> rhs1 = {1, 2};

        std::vector<std::array<unsigned int, 2>> lhs = {lhs1, lhs2};
        std::vector<std::array<unsigned int, 2>> rhs = {rhs1};
        std::vector<std::shared_ptr<RewriteSystem::Rule>> trueRule = {
            std::make_shared<RewriteSystem::Rule>(3.0, lhs, rhs)};

        REQUIRE(rs->get_mapping().size() == trueMap.size());
        for (size_t i = 0; i < trueMap.size(); i++) {
          REQUIRE(rs->get_mapping()[i] == trueMap[i]);
        }
        REQUIRE(rs->get_species_list().size() == trueSpecies.size());
        for (size_t i = 0; i < trueSpecies.size(); i++) {
          REQUIRE(rs->get_species_list()[i] == trueSpecies[i]);
        }
        REQUIRE(rs->get_rules()[0]->equivalent(trueRule[0]));
      }
    }
  }
}

SCENARIO("Parsing an example file yields the correctly initialized data "
         "structures") {
  GIVEN("The example form the PNAS paper, Figure 1 as a file in erode "
        "compliant format") {
    std::string input =
        UserInterface::read_file("../src/test/stoichometric_input.txt");
    WHEN("parsing it") {
      auto model = std::make_shared<RewriteSystemModel>();
      auto repr = model->parse(input);
      auto rs = std::static_pointer_cast<RewriteSystem>(repr);
      THEN("then corresponding data structures are properly initialized") {
        std::vector<std::string> mapping = {"Auu", "Apu", "Aup", "B"};
        std::vector<std::vector<unsigned int>> speciesList = {
            {1, 0, 0, 0}, {0, 1, 0, 0},
            {0, 0, 1, 0}, {0, 0, 0, 1}, // Auu, Apu, Aup, B
            {0, 1, 0, 1}, {0, 0, 1, 1}  // ApuB, AupB
        };

        double k1 = 1.0;
        double k2 = 2.0;
        double k3 = 3.0;
        double k4 = 4.0;
        std::vector<std::array<unsigned int, 2>> lhs1 = {{1, 0}};
        std::vector<std::array<unsigned int, 2>> lhs2 = {{1, 1}};
        std::vector<std::array<unsigned int, 2>> lhs3 = {{1, 0}};
        std::vector<std::array<unsigned int, 2>> lhs4 = {{1, 2}};
        std::vector<std::array<unsigned int, 2>> lhs5 = {{1, 1}, {1, 3}};
        std::vector<std::array<unsigned int, 2>> lhs6 = {{1, 4}};
        std::vector<std::array<unsigned int, 2>> lhs7 = {{1, 2}, {1, 3}};
        std::vector<std::array<unsigned int, 2>> lhs8 = {{1, 5}};

        std::vector<std::array<unsigned int, 2>> rhs1 = {{1, 1}};
        std::vector<std::array<unsigned int, 2>> rhs2 = {{1, 0}};
        std::vector<std::array<unsigned int, 2>> rhs3 = {{1, 2}};
        std::vector<std::array<unsigned int, 2>> rhs4 = {{1, 0}};
        std::vector<std::array<unsigned int, 2>> rhs5 = {{1, 4}};
        std::vector<std::array<unsigned int, 2>> rhs6 = {{1, 1}, {1, 3}};
        std::vector<std::array<unsigned int, 2>> rhs7 = {{1, 5}};
        std::vector<std::array<unsigned int, 2>> rhs8 = {{1, 2}, {1, 3}};

        std::vector<std::shared_ptr<RewriteSystem::Rule>> rules = {
            std::make_shared<RewriteSystem::Rule>(k1, lhs1, rhs1),
            std::make_shared<RewriteSystem::Rule>(k2, lhs2, rhs2),
            std::make_shared<RewriteSystem::Rule>(k1, lhs3, rhs3),
            std::make_shared<RewriteSystem::Rule>(k2, lhs4, rhs4),
            std::make_shared<RewriteSystem::Rule>(k3, lhs5, rhs5),
            std::make_shared<RewriteSystem::Rule>(k4, lhs6, rhs6),
            std::make_shared<RewriteSystem::Rule>(k3, lhs7, rhs7),
            std::make_shared<RewriteSystem::Rule>(k4, lhs8, rhs8)};

        REQUIRE(rs->get_mapping().size() == mapping.size());
        for (size_t i = 0; i < mapping.size(); i++) {
          REQUIRE(rs->get_mapping()[i] == mapping[i]);
        }
        REQUIRE(rs->get_species_list().size() == speciesList.size());
        for (size_t i = 0; i < speciesList.size(); i++) {
          REQUIRE(rs->get_species_list()[i] == speciesList[i]);
        }
        REQUIRE(rs->get_rules().size() == rules.size());
        for (size_t i = 0; i < rules.size(); i++) {
          REQUIRE(rs->get_rules()[i]->equivalent(rules[i]));
        }
      }
    }
  }
}

SCENARIO("Initializations of the necessary data structures for reduction works "
         "as intended") {
  GIVEN("Parsed input") {
    std::string input =
        UserInterface::read_file("../src/test/stoichometric_input.txt");
    auto model = std::make_shared<RewriteSystemModel>();
    auto repr = model->parse(input);
    auto rs = std::static_pointer_cast<RewriteSystem>(repr);
    WHEN("calling init") {
      MaximalAggregation reduction = MaximalAggregation();
      std::vector<unsigned int> initBlock = {};
      for (unsigned int i = 0; i < rs->get_mapping().size(); i++) {
        initBlock.emplace_back(i);
      }
      std::vector<std::vector<unsigned int>> initPart = {initBlock};
      reduction.set_part(initPart);
      reduction.set_system(rs);
      reduction.init();

      THEN("initializes all data strucures to the values expected") {
        std::vector<std::vector<std::array<unsigned int, 2>>>
            truePossibleReagents = {{}, {{1, 1}}, {{1, 2}}, {{1, 3}}};
        std::vector<std::vector<long>> trueLabelPos = {
            {0L}, {0L}, {0L}, {0L}, {3L, 1L}, {0L}, {3L, 2L}, {0L}};
        std::vector<std::vector<size_t>> trueLhsContainsSpec = {
            {0, 2}, {1, 4}, {3, 6}, {4, 6}, {5}, {7}};
        std::vector<std::vector<std::tuple<size_t, double>>> trueNZF = {
            {{0, -1.0}, {1, 2.0}, {2, -1.0}, {3, 2.0}},
            {{0, 1.0}, {1, -2.0}, {4, -3.0}, {5, 4.0}},
            {{2, 1.0}, {3, -2.0}, {6, -3.0}, {7, 4.0}},
            {{4, -3.0}, {5, 4.0}, {6, -3.0}, {7, 4.0}},
            {{4, 3.0}, {5, -4.0}},
            {{6, 3.0}, {7, -4.0}}};

        std::vector<unsigned long> trueMcoeffs = {1, 1, 1, 1, 2, 1, 2, 1};

        const std::set<std::vector<std::array<unsigned int, 2>>> actualLabels =
            reduction.get_reactant_labels();
        REQUIRE(truePossibleReagents.size() == actualLabels.size());
        size_t k = 0;
        for (const auto &elem : actualLabels) {
          REQUIRE(MaximalAggregation::equal_reagents(elem,
                                                     truePossibleReagents[k]));
          k++;
        }

        std::vector<std::vector<long>> labelPos = reduction.get_label_pos();
        REQUIRE(labelPos.size() == trueLabelPos.size());
        for (size_t i = 0; i < labelPos.size(); i++) {
          REQUIRE(labelPos[i].size() == trueLabelPos[i].size());
          for (size_t j = 0; j < labelPos[i].size(); j++) {
            REQUIRE(labelPos[i][j] == trueLabelPos[i][j]);
          }
        }

        std::vector<std::vector<size_t>> lhsContainsSpec =
            reduction.get_lhs_contains_spec();
        REQUIRE(lhsContainsSpec.size() == trueLhsContainsSpec.size());
        for (size_t i = 0; i < lhsContainsSpec.size(); i++) {
          REQUIRE(lhsContainsSpec[i].size() == trueLhsContainsSpec[i].size());
          for (size_t j = 0; j < lhsContainsSpec[i].size(); j++) {
            REQUIRE(lhsContainsSpec[i][j] == trueLhsContainsSpec[i][j]);
          }
        }

        std::vector<std::vector<std::tuple<size_t, double>>> nZF =
            reduction.get_non_zero_flux_rules();
        REQUIRE(nZF.size() == trueNZF.size());
        for (size_t i = 0; i < nZF.size(); i++) {
          REQUIRE(nZF[i].size() == trueNZF[i].size());
          for (size_t j = 0; j < nZF[i].size(); j++) {
            REQUIRE(std::get<0>(nZF[i][j]) == std::get<0>(trueNZF[i][j]));
            REQUIRE(floating_point_compare<double>(std::get<1>(nZF[i][j]),
                                                   std::get<1>(trueNZF[i][j])));
          }
        }

        std::vector<unsigned long> mcoeffs = reduction.get_mcoeffs();
        REQUIRE(mcoeffs.size() == trueMcoeffs.size());
        for (size_t i = 0; i < mcoeffs.size(); i++) {
          REQUIRE(mcoeffs[i] == trueMcoeffs[i]);
        }
      }
    }
  }
}

SCENARIO("Test Parsing Helpers") {
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
    WHEN("extracting the atomic names (i.e. of all Strings that start with a "
         "capital letter followed by one or more lower letters") {
      THEN("it parses then successfully") {
        std::string name;
        for (size_t i = 0; i < result.size(); i++) {
          name = extract_atomic_name(input);
          REQUIRE(result[i] == name);
        }
      }
    }
  }
}

SCENARIO("Test the split predicate") {
  GIVEN(
      "The example from the PNAS paper, parsed and the reduction initialized") {
    std::string input =
        UserInterface::read_file("../src/test/stoichometric_input.txt");
    auto model = std::make_shared<RewriteSystemModel>();
    auto repr = model->parse(input);
    auto rs = std::static_pointer_cast<RewriteSystem>(repr);

    MaximalAggregation reduction = MaximalAggregation();
    std::vector<unsigned int> initBlock = {};
    for (unsigned int i = 0; i < rs->get_species_list().size(); i++) {
      initBlock.emplace_back(i);
    }
    std::vector<std::vector<unsigned int>> initPart = {initBlock};
    reduction.set_part(initPart);
    reduction.set_system(rs);
    reduction.init();
    WHEN("computing the forward rate") {
      std::set<unsigned int> nZRS = std::set<unsigned int>();
      for (size_t i = 0; i < initBlock.size(); i++) {
        reduction.compute_forward_rate(initBlock[i], nZRS);
        std::cout << *(reduction.get_m()) << std::endl;
      }
      THEN("the correct values are computed") {
        MatDenDPtr M = std::make_shared<MatDenD>(
            reduction.get_system()->get_species_list().size(),
            reduction.get_reactant_labels().size());
        *M << 0, 0, 0, 0, 0, 0, 0, -3.0 / 2.0, 0, 0, 0, -3.0 / 2.0, 0,
            -3.0 / 2.0, -3.0 / 2.0, 0, 4, 0, 0, 0, 4, 0, 0, 0;
        std::cout << *(reduction.get_m()) << std::endl;
        std::cout << "True:\n" << *M << std::endl;
        REQUIRE(
            floating_point_compare((*(reduction.get_m()) - *M).norm(), 0.0));
      }
    }

    WHEN("computing the backward rate") {
      THEN("the correct values are computed") {}
    }
  }
}

SCENARIO(
    "A full forward run yields the resulting system given in the PNAS paper") {
  GIVEN(
      "The example from the PNAS paper, parsed and the reduction initialized") {
    std::string input =
        UserInterface::read_file("../src/test/stoichometric_input.txt");
    auto model = std::make_shared<RewriteSystemModel>();
    auto repr = model->parse(input);
    auto rs = std::static_pointer_cast<RewriteSystem>(repr);

    MaximalAggregation reduction = MaximalAggregation();
    std::vector<unsigned int> initBlock = {};
    for (unsigned int i = 0; i < rs->get_species_list().size(); i++) {
      initBlock.emplace_back(i);
    }
    std::vector<std::vector<unsigned int>> initPart = {initBlock};
    reduction.set_part(initPart);
    reduction.set_system(rs);
    reduction.init();
    WHEN("computing the forward reduction") {
      reduction.largest_equivalent_parition(true);
      reduction.apply_reduction();
      THEN("the correct reductions are computed") {}
    }
  }
}

SCENARIO("A full run Seg-faults") {
  GIVEN("Input") {
    std::string input =
        UserInterface::read_file("../src/test/stoichometric_input.txt");
    auto model = std::make_shared<RewriteSystemModel>();
    WHEN("parsing the input") {
      auto repr = model->parse(input);
      auto reduction = model->get_reduction_methods()[0];
      auto reduced = reduction->reduce(repr);
      THEN("it parses") { REQUIRE(true); }
    }
  }
}
