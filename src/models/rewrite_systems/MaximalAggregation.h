#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H

#include "../ReductionMethodInterface.h"
#include "RewriteSystem.h"

#include <mutex>
#include <set>
#include <tuple>
#include <vector>

class MaximalAggregation : public ReductionMethodInterface {
private:
  // The rewrite System under consideration
  std::shared_ptr<RewriteSystem> system;
  // current partition: vector holding vectors of indices of the species
  std::vector<std::vector<unsigned int>> part;
  // holds labels of the partitions
  std::vector<long> partLabels;
  // holds multinomial coefficients for each reaction
  std::vector<unsigned long long int> mcoeffs;
  // holds all reactants to be considered when computing fr/br; species, reagent
  std::set<std::tuple<unsigned int,
                      std::vector<std::shared_ptr<RewriteSystem::Term>>>>
      reactantLabels;
  // gives the position of the labels to be considered per species
  std::vector<std::vector<long>> labelPos;
  // holds all rules with non-zero net flux per species
  std::vector<std::vector<std::tuple<size_t, double>>>
      nonZeroFluxRulesPerSpecies;
  // holds all rules having a certain species in their lhs
  std::vector<std::vector<size_t>> lhsContainsSpec;
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
    const std::vector<std::shared_ptr<RewriteSystem::Rule>> &rules =
        this->system->get_rules();
    const std::vector<std::vector<unsigned int>> &speciesList =
        this->system->get_species_list();

    //#pragma omp parallel default(none)
    //    {
    //#pragma omp single default(none)
    //      {
    //#pragma omp task default(none) shared(rules)
    //{
    this->set_mcoeffs();
    //}

    //#pragma omp task default(none) shared(rules)
    //{
    this->partLabels = std::vector<long>(this->part.size(), -1L);
    this->M =
        std::make_shared<MatDenD>(speciesList.size(), reactantLabels.size());
    this->M->fill(0.0);
    // }

    std::vector<long> pos;
    std::tuple<
        std::set<std::tuple<unsigned int, std::vector<std::shared_ptr<
                                              RewriteSystem::Term>>>>::iterator,
        bool>
        insertTuple;
    std::vector<std::shared_ptr<RewriteSystem::Term>> reagents;
    std::vector<std::shared_ptr<RewriteSystem::Term>> alteredReagents;
    std::mutex labelPosMutex = std::mutex();
    std::mutex reactantLabelsMutex = std::mutex();
    //#pragma omp taskloop default(none) num_threads(THREADS) if (!TEST)             \
//    shared(rules, labelPosMutex,                                      \
//           reactantLabelsMutex) private(pos, diff, candidate)
    for (size_t i = 0; i < rules.size(); i++) {
      reagents = rules[i]->get_lhs();
      pos = std::vector<long>(reagents.size());
      for (size_t k = 0; k < reagents.size(); k++) {
        // generate reduced reagents
        alteredReagents = {};
        for (const auto &elem : reagents) {
          alteredReagents.emplace_back(std::make_shared<RewriteSystem::Term>(
              elem->get_factor(), elem->get_species()));
        }
        alteredReagents[k]->decrement_factor();

        std::lock_guard<std::mutex> guard(reactantLabelsMutex);
        insertTuple = this->reactantLabels.emplace(
            std::make_tuple(reagents[k]->get_species(), alteredReagents));
        pos[k] = std::distance(this->reactantLabels.begin(),
                               std::get<0>(insertTuple));
        this->lhsContainsSpec[reagents[k]->get_species()].emplace_back(i);
      }
      std::lock_guard<std::mutex> guard(labelPosMutex);
      this->labelPos[i] = pos;
    }

    double ns;
    std::mutex nonZeroFluxMutex = std::mutex();
    //#pragma omp taskloop default(none) num_threads(THREADS) if (!TEST)             \
//    shared(rules, speciesList, nonZeroFluxMutex,                                      \
//           ) private(ns)
    for (unsigned int i = 0; i < speciesList.size(); i++) {
      for (size_t j = 0; j < rules.size(); j++) {
        ns = get_factor_for_species(i, rules[i]->get_rhs()) -
             get_factor_for_species(i, rules[i]->get_lhs());
        if (ns > 0.0) {
          std::lock_guard<std::mutex> guard(nonZeroFluxMutex);
          this->nonZeroFluxRulesPerSpecies[i].emplace_back(
              i, ns * rules[i]->get_rate());
        }
      }
    }
    //}
    //}
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

  void split(bool forward, std::vector<unsigned int> &currentSplitter,
             std::vector<std::vector<unsigned int>> &split) {
    // of species with non-zero
    std::set<unsigned int> nonZeroRateSpecies = std::set<unsigned int>();
    std::vector<unsigned int> partitionsContainingNZRS = {};

    for (const auto &sj : currentSplitter) {
      if (forward) {
        compute_forward_rate(sj, nonZeroRateSpecies);
      } else {
        compute_backward_rate(sj, currentSplitter, nonZeroRateSpecies);
      }
    }
    std::vector<unsigned int> marked = {};
    size_t blockNo;
    for (const auto &element : nonZeroRateSpecies) {
      blockNo = get_block_number(element);
      this->partLabels[blockNo] = -1;
      if (!floating_point_compare(this->M->row(element).norm(), 0.0)) {
        if (!contains_marked(marked, blockNo)) {
          partitionsContainingNZRS.emplace_back(blockNo);
        }
      }
      marked.push_back(element);
    }

    bool isSplitter = false;
    std::vector<std::vector<unsigned int>> newBlocks;
    std::vector<unsigned int> currentPartition;
    std::vector<unsigned int> newPartition = {};
    while (!partitionsContainingNZRS.empty()) {
      blockNo = partitionsContainingNZRS.back();
      partitionsContainingNZRS.pop_back();
      currentPartition = this->part[blockNo];
      for (size_t i = 0; i < currentPartition.size(); i++) {
        for (size_t j = 0; j < marked.size(); j++) {
          if (currentPartition[i] == marked[j]) {
            newPartition.push_back(currentPartition[i]);
            currentPartition.erase(currentPartition.begin() +
                                   static_cast<long>(i));
          }
        }
      }
      newBlocks = split_marked(newPartition);

      if (currentPartition.empty()) {
        this->part[blockNo] = newPartition;
        this->partLabels[blockNo] = -1L;
      } else {
        this->part.push_back(newPartition);
        this->partLabels.emplace_back(-1L);
        // check if we the current partition is contained in the splitters
        for (const auto &block : split) {
          if (equal_block(block, this->part[blockNo])) {
            isSplitter = true;
          }
        }
        if (!isSplitter) {
          split.emplace_back(this->part[blockNo]);
        }
      }
      for (size_t j = 0; j < newBlocks.size(); j++) {
        split.emplace_back(newBlocks[j]);
        if (j != 0) {
          this->part.emplace_back(newBlocks[j]);
          this->partLabels.emplace_back(-1L);
        }
      }
    }
    unsigned int currentSpecies;
    Eigen::RowVectorXd zeroVect = Eigen::MatrixXd::Zero(
        1, static_cast<long>(this->reactantLabels.size()));
    while (!nonZeroRateSpecies.empty()) {
      currentSpecies = *nonZeroRateSpecies.rbegin();
      nonZeroRateSpecies.erase(std::prev(nonZeroRateSpecies.end()));
      this->M->row(currentSpecies) = zeroVect;
    }
  }

  inline auto split_marked(std::vector<unsigned int> &markedPartition) const
      -> std::vector<std::vector<unsigned int>> {
    // Asumption: nothing added to partition or splitters so far
    std::vector<std::vector<unsigned int>> result = {};
    std::vector<unsigned int> newBlock;
    // used to cache the indexes of the element to delete to avoid
    //  1. changing the collection that is iterated over while iterating (UB)
    //  2. avoid another iteration or a set difference to look up the indexes
    std::vector<size_t> indexesToDelete;
    double classRate;
    std::vector<unsigned int> &currentBlock = markedPartition;

    while (!currentBlock.empty()) {
      classRate = this->M->row(currentBlock[0]).sum();
      newBlock = {};
      indexesToDelete = {};
      for (size_t i = 0; i < currentBlock.size(); i++) {
        if (!floating_point_compare<double>(
                this->M->row(static_cast<long>(currentBlock[i])).sum(),
                classRate)) {
          newBlock.push_back(currentBlock[i]);
          indexesToDelete.push_back(i);
        }
      }
      for (const auto &index : indexesToDelete) {
        currentBlock.erase(currentBlock.begin() + static_cast<long>(index));
      }
      result.emplace_back(currentBlock);
      currentBlock = newBlock;
    }
    return result;
  }

  void compute_forward_rate(unsigned int species,
                            std::set<unsigned int> &nonZeroRateSpecies) {
    size_t ruleIdx;
    std::shared_ptr<RewriteSystem::Rule> rule;
    double rate;
    unsigned long long int mcoeff;
    std::vector<long> pos;
    std::vector<std::shared_ptr<RewriteSystem::Term>> terms;
    for (size_t i = 0; i < this->nonZeroFluxRulesPerSpecies[species].size();
         i++) {
      ruleIdx = std::get<0>(this->nonZeroFluxRulesPerSpecies[species][i]);
      rule = this->system->get_rules()[ruleIdx];
      rate = std::get<1>(this->nonZeroFluxRulesPerSpecies[species][i]);
      mcoeff = this->mcoeffs[ruleIdx];
      pos = this->labelPos[ruleIdx];
      terms = rule->get_lhs();
      for (size_t j = 0; j < terms.size(); j++) {
        update_m(terms[j]->get_species(), pos[j],
                 rate / static_cast<double>(mcoeff), nonZeroRateSpecies);
      }
    }
  }
  void compute_backward_rate(unsigned int species,
                             std::vector<unsigned int> &currentSplitter,
                             std::set<unsigned int> &nonZeroRateSpecies) {
    std::shared_ptr<RewriteSystem::Rule> rule;
    std::vector<std::shared_ptr<RewriteSystem::Term>> lhs;
    std::vector<std::shared_ptr<RewriteSystem::Term>> rhs;
    double alpha;
    size_t ruleIdx;
    size_t termIdx = 0;
    std::vector<std::shared_ptr<RewriteSystem::Term>> alteredReagents;
    unsigned int ct;
    std::vector<std::shared_ptr<RewriteSystem::Term>> rhoMDash;
    size_t partNo;
    unsigned int currentSpecies;
    long col = -1;
    bool found = false;

    for (size_t i = 0; i < this->lhsContainsSpec[species].size(); i++) {
      ruleIdx = this->lhsContainsSpec[species][i];
      rule = this->system->get_rules()[ruleIdx];
      lhs = rule->get_lhs();
      // We know that the rule must contain the species, i.e. posIdx is well
      // defined after the loop
      for (size_t j = 0; i < lhs.size(); j++) {
        if (lhs[j]->get_species() == species) {
          termIdx = j;
        }
      }
      // retrieve the rule with altered reactants for sj from the list of labels
      // using pos as it saves per rule the position of the label where the j-th
      // term was reduced by 1, i.e. pos[RuleIdx][termIdx] is the index to the
      // label where Sj was subtracted from
      alteredReagents = std::get<1>(*(std::next(
          this->reactantLabels.begin(), this->labelPos[ruleIdx][termIdx])));
      ct = 1;
      rhoMDash = {};
      for (size_t j = 0; j < alteredReagents.size(); j++) {
        currentSpecies = alteredReagents[j]->get_species();
        partNo = this->get_block_number(currentSpecies);
        if (this->equal_block(this->part[partNo], currentSplitter) &&
            currentSpecies != species) {
          ct++;
        }
        if (this->partLabels[partNo] == -1L) {
          this->partLabels[partNo] = currentSpecies;
        }
        // Error prone?

        for (size_t k = 0; k < rhoMDash.size(); k++) {
          if (rhoMDash[k]->get_species() == this->partLabels[partNo]) {
            found = true;
            rhoMDash[k]->add_factor(alteredReagents[j]->get_factor());
            break;
          }
        }
        if (!found) {
          rhoMDash.push_back(std::make_shared<RewriteSystem::Term>(
              alteredReagents[j]->get_factor(), this->partLabels[partNo]));
        }
      }
      // Optimize me
      long k = 0;
      for (const auto &label : this->reactantLabels) {
        if (equal_reagents(std::get<1>(label), rhoMDash)) {
          col = k;
          break;
        }
        k++;
      }
      if (col == -1) {
        std::cout << "rhoMDash in compute br was not found in the labels"
                  << std::endl;
      }

      alpha = rule->get_rate();
      for (size_t j = 0; j < alteredReagents.size(); j++) {
        update_m(alteredReagents[j]->get_species(), col,
                 -alpha * alteredReagents[j]->get_factor() / ct,
                 nonZeroRateSpecies);
      }
      rhs = rule->get_rhs();
      for (size_t j = 0; j < rhs.size(); j++) {
        update_m(rhs[j]->get_species(), col, alpha * rhs[j]->get_factor() / ct,
                 nonZeroRateSpecies);
      }
    }
  }

  std::shared_ptr<RewriteSystem> apply_reduction() {


  }

  inline void update_m(unsigned int species, long colIdx, double val,
                       std::set<unsigned int> &nonZeroRateSpecies) {
    nonZeroRateSpecies.insert(species);
    long rowIdx = static_cast<long>(species);

    this->M->coeffRef(rowIdx, colIdx) = this->M->coeff(rowIdx, colIdx) + val;
  }

  void set_mcoeffs() {
    unsigned int upper = 0;
    unsigned long long int lower;
    const std::vector<std::shared_ptr<RewriteSystem::Rule>> &rules =
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

  inline auto get_factor_for_species(
      unsigned int species,
      const std::vector<std::shared_ptr<RewriteSystem::Term>> &hs)
      -> unsigned int {
    for (const auto &term : hs) {
      if (term->get_species() == species) {
        return term->get_factor();
      }
    }
    return 0;
  }

  inline auto get_block_number(unsigned int species) -> size_t {
    for (size_t i = 0; i < this->part.size(); i++) {
      for (const auto &index : this->part[i]) {
        if (index == species) {
          return i;
        }
      }
    }
    throw std::logic_error(
        "The species must be contained in one of the partitions!");
  }

  inline auto equal_block(const std::vector<unsigned int> &part0,
                          const std::vector<unsigned int> &part1) -> bool {
    if (part0.size() != part1.size()) {
      return false;
    }
    bool found = false;
    for (const auto elem0 : part0) {
      for (const auto elem1 : part1) {
        if (elem0 == elem1) {
          found = true;
        }
      }
      if (found == false) {
        return false;
      } else {
        found = false;
      }
    }
    return true;
  }

  inline auto equal_reagents(
      const std::vector<std::shared_ptr<RewriteSystem::Term>> &reagents0,
      const std::vector<std::shared_ptr<RewriteSystem::Term>> &reagents1)
      -> bool {
    if (reagents0.size() != reagents1.size()) {
      return false;
    }
    bool found = false;
    for (const auto &term0 : reagents0) {
      for (const auto &term1 : reagents1) {
        if (term0->get_factor() == term1->get_factor() &&
            term0->get_species() == term1->get_species()) {
          found = true;
        }
      }
      if (found == false) {
        return false;
      } else {
        found = false;
      }
    }
    return true;
  }

  inline auto contains_marked(std::vector<unsigned int> &markedSpec,
                              size_t partNo) -> bool {
    for (const auto &elem : this->part[partNo]) {
      for (const auto &species : markedSpec) {
        if (elem == species) {
          return true;
        }
      }
    }
    return false;
  }
};

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_MAXIMALAGGREGATION_H
