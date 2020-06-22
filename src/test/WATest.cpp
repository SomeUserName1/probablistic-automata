#include <catch2/catch.hpp>

#include "../FloatingPointCompare.h"
#include "../model/weighted_automata/WeightedAutomaton.h"
#include "TestUtils.h"

SCENARIO("Random Vectors have correct shape") {
  GIVEN("An automaton and a number K") {
    auto mu = {std::make_shared<MatDenD>(10, 10)};
    auto automaton = std::make_shared<WeightedAutomaton<MatDenD>>(
        10, 4, std::make_shared<MatDenD>(1, 10), mu,
        std::make_shared<MatDenD>(10, 1), 1);
    uint K = 100;
    WHEN("generating random vectors") {
      std::vector<MatSpDPtr> vectors =
          WeightedAutomaton<MatDenD>::generate_random_vectors(automaton, K);
      THEN("|states| random row vectors of length |characters| are "
           "generated") {
        REQUIRE(vectors.size() == 10);
        for (const auto &vector : vectors) {
          REQUIRE(vector->rows() == 1);
          REQUIRE(vector->cols() == 4);
        }
      }
    }
  }
}

SCENARIO("Subtraction Automaton is constructed") {
  GIVEN("Two automata A and B") {
    auto wa1 = gen_wa_dense();
    std::shared_ptr<WeightedAutomaton<MatDenD>> wa2;
    WHEN("A and B are the (exactly) same automaton") {
      wa2 = gen_wa_dense();
      auto subtractionAutomaton =
          WeightedAutomaton<MatDenD>::create_subtraction_automaton(*wa1, *wa2);
      // TODO powerset of words up to k as function
      std::vector<std::vector<uint>> words = {
          {0},       {1},       {0, 0},    {0, 1},    {1, 0},
          {1, 1},    {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1},
          {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}};
      THEN("The weight of example words is 0 in the subtraction automaton") {
        for (const auto &word : words) {
          REQUIRE(floating_point_compare(
              subtractionAutomaton->process_word(word), 0.0));
        }
      }
    }
    WHEN("A and B do not have the same number of input characters") {
      int states = 4;
      int characters = 3;
      auto alpha = std::make_shared<MatDenD>(1, states);
      *alpha << 1, 0, 0, 0;
      auto eta = std::make_shared<MatDenD>(states, 1);
      *eta << 0, 0, 0, 1;
      auto mu1 = std::make_shared<MatDenD>(states, states);
      *mu1 << 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
      auto mu2 = std::make_shared<MatDenD>(states, states);
      *mu2 << 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0;
      auto mu3 = std::make_shared<MatDenD>(states, states);
      *mu3 << 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0;
      std::vector<MatDenDPtr> mu = {mu1, mu2};
      wa2 = std::make_shared<WeightedAutomaton<MatDenD>>(states, characters,
                                                         alpha, mu, eta, 1);
      auto subtractionAutomaton =
          WeightedAutomaton<MatDenD>::create_subtraction_automaton(*wa1, *wa2);
      std::vector<uint> word = {0, 2, 2, 2};
      THEN("The subtraction automaton is non-zero") {
        REQUIRE(floating_point_compare(subtractionAutomaton->process_word(word),
                                       0.0));
      }
    }
    WHEN("B is an empty automaton") {
      int states = 0;
      int characters = 0;
      auto alpha = std::make_shared<MatDenD>(1, states);
      auto eta = std::make_shared<MatDenD>(states, 1);
      auto mu1 = std::make_shared<MatDenD>(states, states);
      auto mu2 = std::make_shared<MatDenD>(states, states);
      auto mu3 = std::make_shared<MatDenD>(states, states);
      std::vector<MatDenDPtr> mu = {mu1, mu2};
      wa2 = std::make_shared<WeightedAutomaton<MatDenD>>(states, characters,
                                                         alpha, mu, eta, 1);
      auto subtractionAutomaton =
          WeightedAutomaton<MatDenD>::create_subtraction_automaton(*wa1, *wa2);
      std::vector<std::vector<uint>> words = {{0, 1}, {0, 0}};
      THEN("The subtraction automaton is exactly A (checked by probing two "
           "words)") {
        REQUIRE(
            floating_point_compare(subtractionAutomaton->process_word(words[0]),
                                   wa1->process_word(words[0])));
        REQUIRE(
            floating_point_compare(subtractionAutomaton->process_word(words[1]),
                                   wa1->process_word(words[1])));
      }
    }
  }
}

SCENARIO("The equivalence of automata is tested") {
  GIVEN("Two Automata A and B") {
    auto wa1 = gen_wa_dense();
    std::shared_ptr<WeightedAutomaton<MatDenD>> wa2;
    WHEN("A and B are the exact same automata") {
      wa2 = gen_wa_dense();
      THEN("They are equal") { REQUIRE(wa1->equivalent(wa2)); }
    }
    WHEN("The automata have differing numbers of input characters") {
      int states = 4;
      int characters = 3;
      auto alpha = std::make_shared<MatDenD>(1, states);
      *alpha << 1, 0, 0, 0;
      auto eta = std::make_shared<MatDenD>(states, 1);
      *eta << 0, 0, 0, 1;
      auto mu1 = std::make_shared<MatDenD>(states, states);
      *mu1 << 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
      auto mu2 = std::make_shared<MatDenD>(states, states);
      *mu2 << 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0;
      auto mu3 = std::make_shared<MatDenD>(states, states);
      *mu3 << 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0;
      std::vector<MatDenDPtr> mu = {mu1, mu2};
      wa2 = std::make_shared<WeightedAutomaton<MatDenD>>(states, characters,
                                                         alpha, mu, eta, 1);

      THEN("They are not equal") { REQUIRE(!wa1->equivalent(wa2)); }
    }
    WHEN(
        "The automata are different in structure but equivalent in semantics") {
      int states = 3;
      int characters = 2;
      auto alpha = std::make_shared<MatDenD>(1, states);
      *alpha << 2, 0, 0;
      auto eta = std::make_shared<MatDenD>(states, 1);
      *eta << 0, 0, 1;
      auto mu1 = std::make_shared<MatDenD>(states, states);
      *mu1 << 0, 1, 0, 0, 0, 0, 0, 0, 0;
      auto mu2 = std::make_shared<MatDenD>(states, states);
      *mu2 << 0, 0, 0, 0, 0, 1, 0, 0, 0;
      std::vector<MatDenDPtr> mu = {mu1, mu2};
      wa2 = std::make_shared<WeightedAutomaton<MatDenD>>(states, characters,
                                                         alpha, mu, eta, 1);
      THEN("They are equivalent") { REQUIRE(wa1->equivalent(wa2)); }
    }
    WHEN("The Automata differ in semantics") {
      int states = 3;
      int characters = 2;
      auto alpha = std::make_shared<Eigen::MatrixXd>(1, states);
      *alpha << 2, 0, 0;
      auto eta = std::make_shared<Eigen::MatrixXd>(states, 1);
      *eta << 0, 0, 1;
      auto mu1 = std::make_shared<Eigen::MatrixXd>(states, states);
      *mu1 << 0, 1, 1, 1, 1, 1, 1, 1, 0;
      auto mu2 = std::make_shared<Eigen::MatrixXd>(states, states);
      *mu2 << 0, 0, 0, 0, 0, 1, 0, 0, 0;
      std::vector<MatDenDPtr> mu = {mu1, mu2};
      wa2 = std::make_shared<WeightedAutomaton<Eigen::MatrixXd>>(
          states, characters, alpha, mu, eta, 1);
      THEN("They are not equivalent") { REQUIRE(!wa1->equivalent(wa2)); }
    }
    WHEN("A minimized automaton is compared with the base case") {
      wa2 = gen_wa_min_dense();
      THEN("They're equivalent") { REQUIRE(wa1->equivalent(wa2)); }
    }
  }
}
