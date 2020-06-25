#include <catch2/catch.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#include "../model/weighted_automata/WeightedAutomatonModel.h"
#include "../util/FloatingPointCompare.h"
#include "TestUtils.h"

SCENARIO(
    "The word factors are calculated correctly as specified in the paper") {
  GIVEN("Fixed random vectors") {
    auto mat = gen_fixed_rand_v();
    WHEN("Calculating the word factor") {
      double factor1 =
          KieferSchuetzenbergerReduction<MatDenD>::get_word_factor({0}, mat[0]);
      double factor2 = KieferSchuetzenbergerReduction<MatDenD>::get_word_factor(
          {0, 1}, mat[0]);

      double factor3 =
          KieferSchuetzenbergerReduction<MatDenD>::get_word_factor({0}, mat[1]);
      double factor4 = KieferSchuetzenbergerReduction<MatDenD>::get_word_factor(
          {0, 1}, mat[1]);

      double factor5 =
          KieferSchuetzenbergerReduction<MatDenD>::get_word_factor({0}, mat[2]);
      double factor6 = KieferSchuetzenbergerReduction<MatDenD>::get_word_factor(
          {0, 1}, mat[2]);

      double factor7 =
          KieferSchuetzenbergerReduction<MatDenD>::get_word_factor({0}, mat[3]);
      double factor8 = KieferSchuetzenbergerReduction<MatDenD>::get_word_factor(
          {0, 1}, mat[3]);

      THEN("Is the same as when computed by hand") {
        REQUIRE(floating_point_compare(factor1, 9.0));
        REQUIRE(floating_point_compare(factor2, 99.0));

        REQUIRE(floating_point_compare(factor3, 2.0));
        REQUIRE(floating_point_compare(factor4, 6.0));

        REQUIRE(floating_point_compare(factor5, 2.0));
        REQUIRE(floating_point_compare(factor6, 22.0));

        REQUIRE(floating_point_compare(factor7, 4.0));
        REQUIRE(floating_point_compare(factor8, 36.0));
      }
    }
  }
}
SCENARIO("The random vectors have the correct shape") {
  GIVEN("An automaton A and a number K") {
    auto mu = {std::make_shared<MatDenD>(10, 10)};
    auto automaton = std::make_shared<WeightedAutomaton<MatDenD>>(
        10, 4, std::make_shared<MatDenD>(1, 10), mu,
        std::make_shared<MatDenD>(10, 1));
    uint K = 100;
    WHEN("generating random vectors") {
      auto vectors =
          KieferSchuetzenbergerReduction<MatDenD>::generate_random_vectors(
              automaton, K);
      THEN("yields |states| random matrices of shape |characters| * |states| "
           "are generated") {
        REQUIRE(vectors.size() == 10);
        for (const auto &vector : vectors) {
          REQUIRE(vector->rows() == 4);
          REQUIRE(vector->cols() == 10);
        }
      }
    }
  }
}
SCENARIO("The generating words yields only valid words") {
  GIVEN("An automaton A") {
    auto A = gen_wa_dense();

    WHEN("Generating words forwards") {
      auto wordsForward =
          KieferSchuetzenbergerReduction<MatDenD>::generate_words_forwards(
              A, A->get_states());
      THEN("The generated words and their vectors are the same as when "
           "calculated by hand") {
        REQUIRE(wordsForward.size() == 2);

        auto [vector, word] = wordsForward[0];
        REQUIRE(word.size() == 1);
        REQUIRE(word[0] == 0);
        REQUIRE(vector->rows() == 1);
        REQUIRE(vector->cols() == A->get_states());
        auto hcVector = MatDenD(1, A->get_states());
        hcVector << 0, 1, 1, 0;
        REQUIRE(floating_point_compare((*vector - hcVector).norm(), 0.0));

        std::tie(vector, word) = wordsForward[1];
        REQUIRE(word.size() == 2);
        REQUIRE(word[0] == 0);
        REQUIRE(word[1] == 1);
        REQUIRE(vector->rows() == 1);
        REQUIRE(vector->cols() == A->get_states());
        hcVector << 0, 0, 0, 2;
        REQUIRE(floating_point_compare((*vector - hcVector).norm(), 0.0));
      }
    }
    WHEN("Generating words backwards") {
      auto wordsBackward =
          KieferSchuetzenbergerReduction<MatDenD>::generate_words_backwards(
              A, A->get_states());
      THEN("The generated words and their vectors are the same as when "
           "calculated by hand") {
        REQUIRE(wordsBackward.size() == 2);

        auto [vector, word] = wordsBackward[0];
        REQUIRE(word.size() == 1);
        REQUIRE(word[0] == 1);
        REQUIRE(vector->rows() == A->get_states());
        REQUIRE(vector->cols() == 1);
        auto hcVector = MatDenD(A->get_states(), 1);
        hcVector << 0, 1, 1, 0;
        REQUIRE(floating_point_compare((*vector - hcVector).norm(), 0.0));

        std::tie(vector, word) = wordsBackward[1];
        REQUIRE(word.size() == 2);
        REQUIRE(word[0] == 0);
        REQUIRE(word[1] == 1);
        REQUIRE(vector->rows() == A->get_states());
        REQUIRE(vector->cols() == 1);
        hcVector << 2, 0, 0, 0;
        REQUIRE(floating_point_compare((*vector - hcVector).norm(), 0.0));
      }
    }
  }
}
SCENARIO("The rho vectors are calculated correctly as specified in the paper") {
  GIVEN("An automaton A and fixed random vectors R") {
    auto A = gen_wa_dense();
    auto randomVectors = gen_fixed_rand_v();
    WHEN("calculating the rho vectors forward") {

      std::vector<std::shared_ptr<Eigen::SparseMatrix<double, 0, long>>>
          rhoForward = KieferSchuetzenbergerReduction<
              MatDenD>::calculate_rho_forward_vectors(A, randomVectors);
      THEN("They are the same as when calculating them by hand") {
        MatDenD v1(1, 4);
        MatDenD v2(1, 4);
        MatDenD v3(1, 4);
        MatDenD v4(1, 4);
        v1 << 0, 9, 9, 198;
        v2 << 0, 2, 2, 12;
        v3 << 0, 2, 2, 44;
        v4 << 0, 4, 4, 72;
        std::vector<MatSpDPtr> vect = {
            convert_N_M<MatSpD, MatDenD>(v1), convert_N_M<MatSpD, MatDenD>(v2),
            convert_N_M<MatSpD, MatDenD>(v3), convert_N_M<MatSpD, MatDenD>(v4)};
        for (size_t i = 0; i < A->get_states(); i++) {
          REQUIRE(floating_point_compare((*(vect[i]) - *(rhoForward[i])).norm(),
                                         0.0));
        }
      }
    }
    WHEN("calculating the rho vectors backward") {

      auto rhoBackward = KieferSchuetzenbergerReduction<
          MatDenD>::calculate_rho_backward_vectors(A, randomVectors);
      THEN("They are the same as when calculating them by hand") {
        MatDenD v1(4, 1);
        MatDenD v2(4, 1);
        MatDenD v3(4, 1);
        MatDenD v4(4, 1);
        v1 << 198, 6, 6, 0;
        v2 << 12, 12, 12, 0;
        v3 << 44, 1, 1, 0;
        v4 << 72, 5, 5, 0;
        std::vector<MatSpDPtr> vect = {
            convert_N_M<MatSpD>(v1), convert_N_M<MatSpD>(v2),
            convert_N_M<MatSpD>(v3), convert_N_M<MatSpD>(v4)};

        for (size_t i = 0; i < A->get_states(); i++) {
          REQUIRE(floating_point_compare(
              (*(vect[i]) - *(rhoBackward[i])).norm(), 0.0));
        }
      }
    }
  }
}
SCENARIO("The forward and backward reductions are calculated correctly as "
         "specified in the paper") {
  GIVEN("An automaton A and fixed random vectors R") {
    auto wa = gen_wa_dense();
    auto randV = gen_fixed_rand_v();
    WHEN("calculating the forward reduction") {
      auto minWA =
          KieferSchuetzenbergerReduction<MatDenD>::forward_reduction(wa, randV);
      THEN("They are the same as when calculating them by hand") {
        REQUIRE(minWA->get_states() == 3);
        REQUIRE(minWA->get_number_input_characters());

        auto trueAlpha = MatDenD(1, 3);
        trueAlpha << 1, 0, 0;
        REQUIRE(floating_point_compare(
            (*(minWA->get_alpha()) - trueAlpha).norm(), 0.0));

        auto trueMu1 = MatDenD(3, 3);
        auto trueMu2 = MatDenD(3, 3);
        trueMu1 << 0, -1.0 / 24.0, 33.0 / 48.0, 0, 0, 0, 0, 0, 0;
        trueMu2 << 0, 0, 0, 0, 1.0 / 8.0, -9.0 / 16.0, 0, 1.0 / 36.0,
            -1.0 / 8.0;
        std::vector<MatDenD> trueMu = {trueMu1, trueMu2};
        for (size_t i = 0; i < 2; i++) {
          REQUIRE((minWA->get_mu()[i])->isApprox(trueMu[i]));
        }

        auto trueEta = MatDenD(3, 1);
        trueEta << 0, 198, 12;
        REQUIRE(floating_point_compare((*(minWA->get_eta()) - trueEta).norm(),
                                       0.0));
      }
    }
    WHEN("calculating the backward reduction") {

      auto minWA = KieferSchuetzenbergerReduction<MatDenD>::backward_reduction(
          wa, randV);
      THEN("They are the same as when calculating them by hand") {
        REQUIRE(minWA->get_states() == 3);
        REQUIRE(minWA->get_number_input_characters());

        auto trueAlpha = MatDenD(1, 3);
        trueAlpha << 0, 198, 12;
        REQUIRE(floating_point_compare(
            (*(minWA->get_alpha()) - trueAlpha).norm(), 0.0));

        auto trueMu1 = MatDenD(3, 3);
        auto trueMu2 = MatDenD(3, 3);
        trueMu1 << 0, 0, 0, 0, 1.0 / 16.0, 1.0 / 8.0, 0, -1.0 / 32.0,
            -1.0 / 16.0;
        trueMu2 << 0, 0, 0, -1.0 / 192.0, 0, 0, 11.0 / 128.0, 0, 0;
        std::vector<MatDenD> trueMu = {trueMu1, trueMu2};
        for (size_t i = 0; i < 2; i++) {
          REQUIRE((minWA->get_mu()[i])->isApprox(trueMu[i]));
        }

        auto trueEta = MatDenD(3, 1);
        trueEta << 1, 0, 0;
        REQUIRE(floating_point_compare((*(minWA->get_eta()) - trueEta).norm(),
                                       0.0));
      }
    }
  }
}

SCENARIO("When executing the full reduction") {
  GIVEN("an initial automaton and the random vectors") {
    auto A = gen_wa_dense();
    auto randomVectors4 = gen_fixed_rand_v();
    auto randomVectors3 = gen_fixed_rand_v3();
    WHEN("Executing the exact same steps as when calling reduce on an WA") {
      std::shared_ptr<WeightedAutomaton<MatDenD>> minWA =
          KieferSchuetzenbergerReduction<MatDenD>::forward_reduction(
              A, randomVectors4);
      minWA =
          KieferSchuetzenbergerReduction<Eigen::MatrixXd>::backward_reduction(
              minWA, randomVectors3);
      THEN("The result is equivalent to what is calculated by hand") {
        REQUIRE(minWA->get_states() == 3);
        REQUIRE(minWA->get_number_input_characters());

        auto trueAlpha = Eigen::MatrixXd(1, 3);
        trueAlpha << 0, 10, 128;
        REQUIRE((minWA->get_alpha())->isApprox(trueAlpha));

        auto trueMu1 = Eigen::MatrixXd(3, 3);
        auto trueMu2 = Eigen::MatrixXd(3, 3);
        trueMu1 << 5.256737805990320e-20, -1.612066260503700e-18,
            -2.821115955881475e-18, 2.775557561562891e-17, -28.0 / 221.0,
            -49.0 / 221.0, 0, 16.0 / 221.0, 28.0 / 221.0;
        trueMu2 << 2.523234146875356e-18, 9.617563887311330e-17,
            1.193872527811887e-15, 64.0 / 221.0, 2.775557561562891e-17,
            6.661338147750939e-16, -5.0 / 221.0, -3.469446951953614e-18,
            -5.551115123125783e-17;
        std::vector<Eigen::MatrixXd> trueMu = {trueMu1, trueMu2};
        for (size_t i = 0; i < 2; i++) {
          REQUIRE((minWA->get_mu()[i])->isApprox(trueMu[i]));
        }

        auto trueEta = Eigen::MatrixXd(3, 1);
        trueEta << 1, 0, 0;
        REQUIRE((minWA->get_eta())->isApprox(trueEta));
      }
    }
  }
}

SCENARIO("The reduced automaton accepts the same words with the same weights") {
  GIVEN("The running example") {
    auto wa = gen_wa_sparse();
    WHEN("Reducing it using KieferSchuetzenberger") {
      auto reducedRI = KieferSchuetzenbergerReduction<MatSpD>::reduce(wa, 100, false);
      auto reducedWA = static_pointer_cast<WeightedAutomaton<MatSpD>>(reducedRI);
      std::vector<std::vector<unsigned int>> words;
      generate_words(wa->get_states(), wa->get_number_input_characters(),
                     words);
      THEN("Evaluating different words yields the same result") {
        for (const auto &word : words) {
          REQUIRE(floating_point_compare(
              wa->process_word(word) - reducedWA->process_word(word), 0.0));
        }
      }
    }
  }
}
