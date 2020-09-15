#include <stdexcept>
#include <libbear/ea/elements.h>
#include <libbear/ea/genotype.h>
#include <libbear/ea/variation.h>

libbear::population
libbear::stochastic_mutation::
operator()(const genotype& g) const {
  return success(mutation_probability_)
    ? mutate_(g)
    : population{g};
}

libbear::population
libbear::stochastic_recombination::
operator()(const genotype& g0, const genotype& g1) const {
  const auto res_t = recombine_(g0, g1);
  return success(recombination_probability_)
    ? res_t
    : res_t.size() == 2
      ? population{g0, g1}
      : success(.5)
        ? population{g0}
        : population{g1};
}

libbear::population
libbear::variation::
operator()(const genotype& g0, const genotype& g1) const {
  population res{};
  for (const auto& g : recombine_(g0, g1)) {
    res.push_back(mutate_(g).at(0));
  }
  assert(res.size() == 1 || res.size() == 2);
  return res;
}

libbear::population
libbear::variation::
operator()(const population& p) const {
  if (p.size() % 2) {
    throw std::invalid_argument{"variation: wrong population size"};
  }
  population res;
  for (std::size_t i = 0; i < p.size(); i += 2) {
    for (const auto& g : operator()(p[i], p[i + 1])) {
      res.push_back(g);
    }
  }
  assert(res.size() == p.size() / 2 || res.size() == p.size());
  return res;
}

