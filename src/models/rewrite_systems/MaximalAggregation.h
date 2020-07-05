#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H

#include "../ReductionMethodInterface.h"
#include "../rewrite_systems/RewriteSystemModel.h"

#include <list>
#include <vector>

class MaximalAggregation : public ReductionMethodInterface {
private:
  // The rewrite System under consideration
  std::shared_ptr<RewriteSystem> system;
  // current partition: vector holding vectors of indices of the species
  std::vector<std::vector<unsigned int>> part;
  // holds multinomial coefficients for each reaction
  std::vector<unsigned long long int> mcoeffs;
  // holds all reactants to be considered when computing fr/br; species,
  // reactant
  std::set<std::tuple<std::vector<unsigned int>, std::vector<unsigned int>>>
      reactantLabels;
  // gives the position of the labels to be considered per species
  std::vector<std::vector<size_t>> labelPos;
  // holds all rules with non-zero net flux per species
  std::vector<
      std::vector<std::tuple<std::shared_ptr<RewriteSystem::Rule>, double>>>
      nonZeroFluxRulesPerSpecies;
  // holds all rules having a certain species in their lhs
  std::vector<std::vector<std::shared_ptr<RewriteSystem::Rule>>>
      lhsContainsSpec;

public:
  MaximalAggregation() = default;
  ~MaximalAggregation() override;

  [[nodiscard]] std::string MaximalAggregation::get_name() const override {
    return "Maximal Aggregation";
  }

  std::shared_ptr<RepresentationInterface> MaximalAggregation::reduce(
      std::shared_ptr<RepresentationInterface> &ptr) const {
    auto rewriteSystem = static_pointer_cast<RewriteSystem>(ptr);
    std::vector<unsigned int> initBlock = {};
    unsigned int i =
        0 for (const auto &species : rewriteSystem->get_mapping()) {
      initBlock.emplace_back(i);
    }
    std::vector<std::vector<unsigned int>> initPart = {initBlock};
    return this->reduce(rewriteSystem, initPart);
  }

  std::shared_ptr<RewriteSystem>
  MaximalAggregation::reduce(std::shared_ptr<RewriteSystem> &ptr,
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
    this->init(this->system);
    if (!forward) {
      this->backward_prepartitioning(system, initPart);
    }
    std::vector<std::vector<unsigned int>> splitters = this->part;
    unsigned int currentBlock = 0;
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
    std::vector<std::vector<unsigned int>> species =
        this->system->get_species();

#pragma omp parallel default(none)
    {
#pragma omp single default(none)
      {
#pragma omp task default(none) shared(rules)
        { this->set_mcoeffs(); }

        std::vector<size_t> pos;
        std::tuple<std::set<std::tuple<std::vector<unsigned int>,
                                       std::vector<unsigned int>>>::iterator,
                   bool>
            insertTuple;
        std::mutex labelPosMutex = std::mutex();
        std::mutex reactantLabelsMutex = std::mutex();
#pragma omp taskloop default(none) num_threads(THREADS) if (!TEST)             \
    shared(rules, species, labelPosMutex,                                      \
           reactantLabelsMutex) private(pos, diff, candidate)
        for (size_t i = 0; i < rules.size(); i++) {
          pos = std::vector(species.size(), 0);
          for (const auto &term : rules[i]) {
            for (size_t j = 0; j < species.size(); j++) {
              if (term->get_spec() == j) {
                std::lock_guard<std::mutex> guard(reactantLabelsMutex);
                insertTuple =
                    reactantLabels.emplace(species[j], term->get_factor() - 1);
                pos[j] = std::distance(reactantLabels.begin(),
                                       std::get<0>(insertTuple));
              }
            }
          }
          std::lock_guard<std::mutex> guard(labelPosMutex);
          this->labelPos[i] = pos;
        }
      }
    }
    this->M = std::make_shared<MatDenD>(this->system->get_species(),
                                        reactantLabels.size());
  }

  void backward_prepartitioning() {
    std::array<double, this->system->get_species().size()> v;
    v.fill(0.0);
    for (const auto &rule : this->system->get_rules()) {
      if (rule->get_lhs().size() == 0) {
        for (const auto &term : rule->get_rhs()) {
          v[term->get_species()] += term->get_factor() * rule->get_rate();
        }
      }
    }
    std::vector<std::vector<unsigned int> newPart = {};
    std::vector<unsigned int> newBlock;
    for (size_t i = 0; i < this->part.size(); i++) {
      newBlock = {};
      std::sort(std::begin(part[i]), std::end(part[i]), [v]( const auto& lhs, const auto& rhs )
      {
        return v[lhs] < v[rhs];
      });
      double prev = v[part[i][0]];
      newBlock.emplace_back(part[i][0]);
      for (size_t j = 1; j < part[i].size(); j++) {
        if (v[part[i]][j] == prev) {
          newBlock.emplace_back(part[i][j]);
        } else {
          newPart.emplace_back(newBlock);
          newBlock = {};
          prev = v[part[i]][j]
        }
      }
    }
    this->part = newPart;
  }

  void split(bool forward,
             const std::vector<std::vector<unsigned int>> &split) {}

  void compute_forward_reduction();
  void compute_backward_reduction();
  std::shared_ptr<RewriteSystem> apply_reduction();

  void set_mcoeffs() {
    unsigned int upper = 0;
    unsigned long long int lower = 1;
    std::vector<std::shared_ptr<RewriteSystem::Rule>> rules =
        this->system->get_rules();

    for (rule : rules) {
      this->mcoeffs.emplace_back(0);
    }

    std::mutex mcoeffsMutex = std::mutex();
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(this->mcoeffs, mcoeffsMutex, rules) private(upper, lower)
    for (size_t i = 0; i < rules.size(); i++) {
      upper = 0;
      lower = 1;
#pragma omp parallel for default(none) num_threads(THREADS) if (!TEST)         \
    shared(upper, lower, i)
      for (const auto &term : rules[i]->lhs) {
        upper += term->get_factor();
        lower *= FACTORIALS[term->get_factor()];
      }
      std::lock_guard<std::mutex> guard(mcoeffsMutex);
      this->mcoeffs[i] = FACTORIALS[upper] / lower;
    }
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
