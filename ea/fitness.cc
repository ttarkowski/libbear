#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <unordered_set>
#include <libbear/ea/elements.h>
#include <libbear/ea/fitness.h>
#include <libbear/ea/genotype.h>

// TODO: Consider memoization.
libbear::fitness
libbear::fitness_function::
operator()(const genotype& g) const {
  auto it{ fitness_values_->find(g) };
  return it != fitness_values_->end()
    ? it->second
    : ((*fitness_values_)[g] = function_(g));
}
  
libbear::fitnesses
libbear::fitness_function::
operator()(const population& p) const {
  fitnesses res{};
  std::ranges::transform(p, std::back_inserter(res),
                         [this](const genotype& g) { return operator()(g); });
  return res;
}

libbear::fitness_function::unique_genotypes
libbear::fitness_function::uncalculated_fitness(const population& p) const {
  std::unordered_set<genotype> res{};
  std::ranges::copy_if(p, std::inserter(res, std::end(res)),
                       [this](const genotype& g) {
                         return !fitness_values_->contains(g);
                       });
  return res;
}

libbear::selection_probabilities
libbear::fitness_proportional_selection::
operator()(const population& p) const {
  const fitnesses fs{ff_(p)};
  const fitness sum = std::accumulate(std::begin(fs), std::end(fs), fitness{0.});
  selection_probabilities res{};
  std::ranges::transform(fs, std::back_inserter(res),
                         [sum](fitness f) { return f / sum; });
  return res;
}
