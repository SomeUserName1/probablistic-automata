#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H

#include "../ReductionMethodInterface.h"
#include "RewriteSystem.h"

#include <mutex>
#include <set>
#include <vector>

class MaximalAggregation : public ReductionMethodInterface {
private:
  // The rewrite System under consideration
  std::shared_ptr<RewriteSystem> system;
  // current partition: vector holding vectors of indices of the species
  std::vector<std::vector<unsigned int>> part;
  // holds multinomial coefficients for each reaction
  std::vector<unsigned long long int> mcoeffs;
  // holds all reactants to be considered when computing fr/br; species, reagent
  std::set<std::tuple<unsigned int, std::vector<RewriteSystem::Term>>>
      reactantLabels;
  // gives the position of the labels to be considered per species
  std::vector<std::shared_ptr<std::vector<size_t>>> labelPos;
  // holds all rules with non-zero net flux per species
  std::vector<
      std::vector<std::tuple<std::shared_ptr<RewriteSystem::Rule>, double, unsigned int, std::vector<size_t>>>>
      nonZeroFluxRulesPerSpecies;
  // holds all rules having a certain species in their lhs
  std::vector<std::vector<std::shared_ptr<RewriteSystem::Rule>>>
      lhsContainsSpec;
  // Used for caching the forward/backward rate values
  MatDenDPtr M;

public:
  MaximalAggregation() = default;
  ~MaximalAggregation() override;

  [[nodiscard]] std::string get_name() const override {
    return "Maximal Aggregation";
  }

  std::shared_ptr<RepresentationInterface>
  reduce(const std::shared_ptr<RepresentationInterface> &ptr) override {
    auto rewriteSystem = static_pointer_cast<RewriteSystem>(ptr);
    std::vector<unsigned int> initBlock = {};
    for (unsigned int i = 0; i < rewriteSystem->get_mapping().size(); i++) {
      initBlock.emplace_back(i);
    }
    std::vector<std::vector<unsigned int>> initPart = {initBlock};
    return this->reduce(rewriteSystem, initPart);
  }

  std::shared_ptr<RewriteSystem>
  reduce(const std::shared_ptr<RewriteSystem> &ptr,
         std::vector<std::vector<unsigned int>> &initPart) {
    this->system = ptr;
    this->part = initPart;
    this->part = this->largest_equivalent_parition(true);
    this->system = this->apply_reduction();

    this->part = this->largest_equivalent_parition(false);
    this->system = this->apply_reduction();

    return this->system;
  }

  std::vector<std::vector<unsigned int>>
  largest_equivalent_parition(bool forward) {
    this->init();
    if (!forward) {
      this->backward_prepartitioning();
    }
    std::vector<std::vector<unsigned int>> splitters = this->part;
    std::vector<unsigned int> currentBlock;
    while (!splitters.empty()) {
      currentBlock = splitters.back();
      splitters.pop_back();
      this->split(forward, currentBlock, splitters);
    }
    return part;
  }

  // Build labels, pos vector, mcoeff, matrix M
  void init() {
    std::vector<std::shared_ptr<RewriteSystem::Rule>> rules =
        this->system->get_rules();
    std::vector<std::vector<unsigned int>> speciesList =
        this->system->get_species_list();

    //#pragma omp parallel default(none)
    //    {
    //#pragma omp single default(none)
    //      {
    //#pragma omp task default(none) shared(rules)
    //{
    this->set_mcoeffs();
    //}


    std::shared_ptr<std::vector<size_t>> pos;
    std::tuple<std::set<std::tuple<unsigned int, std::vector<Term>>>::iterator,
               bool>
        insertTuple;
    std::vector<std::shared_ptr<RewriteSystem::Term>> reagents;
    std::vector<RewriteSystem::Term> alteredReagents;
    std::mutex labelPosMutex = std::mutex();
    std::mutex reactantLabelsMutex = std::mutex();
    //#pragma omp taskloop default(none) num_threads(THREADS) if (!TEST)             \
//    shared(rules, speciesList, labelPosMutex,                                      \
//           reactantLabelsMutex) private(pos, diff, candidate)
    for (size_t i = 0; i < rules.size(); i++) {
      pos = std::make_shared<std::vector>(speciesList.size());
      reagents = rules[i]->get_lhs();
      for (size_t k = 0; k < reagents.size(); k++) {
        for (size_t j = 0; j < speciesList.size(); j++) {
          if (reagents[k]->get_species() == j) {
            alteredReagents = std::vector(reagents.size());
            for (const auto &elem : reagents) {
              alteredReagents.emplace_back(std::make_shared<Term>(
                  elem->get_factor(), elem->get_species()));
            }
            alteredReagents[k].decrement_factor();

            std::lock_guard<std::mutex> guard(reactantLabelsMutex);
            insertTuple =
                this->reactantLabels.emplace({speciesList[j], alteredReagents});
            (*pos)[j] = static_cast<size_t>(std::distance(
                reactantLabels.begin(), std::get<0>(insertTuple)));
            this->lhsContainsSpec[j].emplace_back(rules[i]);
          }
        }
      }
      std::lock_guard<std::mutex> guard(labelPosMutex);
      this->labelPos[i] = pos;
    }

    double ns;
    std::mutex nonZeroFluxMutex = std::mutex();
    //#pragma omp taskloop default(none) num_threads(THREADS) if (!TEST)             \
//    shared(rules, speciesList, nonZeroFluxMutex,                                      \
//           ) private(ns)
    for (unsigned int i = 0; i < speciesList.size()) {
      for (size_t j = 0; j < rules.size(); j++) {
        ns = get_factor_for_species(speciesList[i], rules[i]->get_rhs()) -
             get_factor_for_species(speciesList[i], rules[i]->get_lhs());
        if (ns > 0.0) {
          std::lock_guard<std::mutex> guard(nonZeroFluxMutex);
          this->nonZeroFluxRulesPerSpecies[i].emplace_back(rule[i], ns * rules[i]->get_rate(), this->mcoeffs[i], this->labelPos[i]);
        }
      }
    }

    //}
    //}
    this->M = std::make_shared<MatDenD>(this->system->get_species_list().size(),
                                        reactantLabels.size());
    this->M->fill(0.0);
  }

  void backward_prepartitioning() {
    std::vector<double> v =
        std::vector(this->system->get_species_list().size(), 0.0);
    for (const auto &rule : this->system->get_rules()) {
      if (rule->get_lhs().size() == 0) {
        for (const auto &term : rule->get_rhs()) {
          v[term->get_species()] +=
              static_cast<double>(term->get_factor()) * rule->get_rate();
        }
      }
    }
    std::vector<std::vector<unsigned int>> newPart = {};
    std::vector<unsigned int> newBlock;
    for (size_t i = 0; i < this->part.size(); i++) {
      newBlock = {};
      std::sort(
          std::begin(part[i]), std::end(part[i]),
          [v](const auto &lhs, const auto &rhs) { return v[lhs] < v[rhs]; });
      double prev = v[part[i][0]];
      newBlock.emplace_back(part[i][0]);
      for (size_t j = 1; j < part[i].size(); j++) {
        if (floating_point_compare<double>(v[part[i][j]], prev)) {
          newBlock.emplace_back(part[i][j]);
        } else {
          newPart.emplace_back(newBlock);
          newBlock = {};
          prev = v[part[i][j]];
        }
      }
    }
    this->part = newPart;
  }

  void split(bool forward, std::vector<unsigned int> &currentBlock,
             const std::vector<std::vector<unsigned int>> &split) {
    // of species with non-zero
    std::set<unsigned int> nonZeroRateSpecies;
    std::set<std::vector<unsigned int>> partitionsContainingNZRS;

    for (const auto& sj : currentBlock) {
      if (forward) {
        compute_forward_rate(sj, nonZeroRateSpecies);
      } else {
        compute_backward_rate(sj, currentBlock, nonZeroRateSpecies);
      }
    }

    for ()
  }

  void compute_forward_rate(unsigned int species, std::set<unsigned int> &nonZeroRateSpecies) {
    std::shared_ptr<RewriteSystem::Rule> rule;
    double rate;
    double mcoeff;
    std::shared_ptr<std::vector<size_t>> pos;
    for (size_t i = 0; i < this->nonZeroFluxRulesPerSpecies[species].size(); i++) {
      rule = std::get<0>(elem);
      rate = std::get<1>(elem);
      mcoeff = std::get<2>(elem);
      pos = std::get<3>(elem);
      for (const auto &term : rule->get_lhs()) {
        update_m(term->get_species(), pos[term->get_species()], rate/mcoeff);
      }
    }
  }
  void compute_backward_rate(unsigned int species, std::vector<unsigned int> currentBlock, std::set<unsigned int> &nonZeroRateSpecies) {}

  std::shared_ptr<RewriteSystem> apply_reduction() {}

  void update_m(unsigned int species, size_t col, double val, std::set<unsigned int> &nonZeroRateSpecies) {
    nonZeroRateSpecies.insert(species);
    this->M->coeffRef(species, col) = this->M->coeff(species, col) + val;
  }

  void set_mcoeffs() {
    unsigned int upper = 0;
    unsigned long long int lower = 1;
    std::vector<std::shared_ptr<RewriteSystem::Rule>> rules =
        this->system->get_rules();

    this->mcoeffs = std::vector(rules.size(), 0uLL);

    std::mutex mcoeffsMutex = std::mutex();
    //#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
//    shared(this->mcoeffs, mcoeffsMutex, rules) private(upper, lower)
    for (size_t i = 0; i < rules.size(); i++) {
      upper = 0;
      lower = 1;
      //#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
//    shared(upper, lower, i)
      for (const auto &term : rules[i]->get_lhs()) {
        upper += term->get_factor();
        lower *= FACTORIALS[term->get_factor()];
      }
      std::lock_guard<std::mutex> guard(mcoeffsMutex);
      this->mcoeffs[i] = FACTORIALS[upper] / lower;
    }
  }

  auto get_factor_for_species(unsigned int species,
                              std::vector<std::shared_ptr<Term>> hs)
      -> unsigned int {
    for (const auto &term : hs) {
      if (term->get_species() == species) {
        return term->get_factor();
      }
    }
    return 0;
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
