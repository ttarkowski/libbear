#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include "elements.h"
#include "fitness.h"
#include "genotype.h"

libbear::fitness_function::
fitness_function(const function& f)
  : function_{f}
{}

// TODO: Consider memoization.
libbear::fitness
libbear::fitness_function::
operator()(const genotype& g) const
{
  auto it{ fitness_values_->find(g) };
  return it != fitness_values_->end()
    ? it->second
    : ((*fitness_values_)[g] = function_(g));
}
  
libbear::fitnesses
libbear::fitness_function::
operator()(const population& p) const
{
  fitnesses res{};
  std::transform(std::begin(p), std::end(p), std::back_inserter(res),
                 [this](const genotype& g) { return operator()(g); });
  return res;
}

std::size_t
libbear::fitness_function::
size() const
{
  return fitness_values_->size();
}

libbear::fitness_proportional_selection::
fitness_proportional_selection(const fitness_function& ff)
  : ff_{ff}
{}

libbear::selection_probabilities
libbear::fitness_proportional_selection::
operator()(const population& p) const
{
  selection_probabilities res{};
  std::transform(std::begin(p), std::end(p), std::back_inserter(res),
                 [this](const auto& g) { return ff_(g); });
  const auto sum = std::accumulate(std::begin(res), std::end(res), fitness{0.});
  std::transform(std::begin(res), std::end(res), std::begin(res),
                 [sum](auto x) { return x / sum; });
  return res;
}