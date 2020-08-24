#include "elements.h"
#include "genotype.h"
#include "variation.h"

libbear::population
libbear::
unary_identity(const genotype& g)
{
  return population{g};
}

libbear::population
libbear::
binary_identity(const genotype& g0, const genotype& g1)
{
  return population{g0, g1};
}


libbear::stochastic_mutation::
stochastic_mutation(const mutation_fn& m, probability p)
  : mutate_{m}
  , mutation_probability_{p}
{}

libbear::population
libbear::stochastic_mutation::
operator()(const genotype& g) const
{
  return success(mutation_probability_)
    ? mutate_(g)
    : population{g};
}
  
libbear::stochastic_recombination::
stochastic_recombination(const recombination_fn& r, probability p)
  : recombine_{r}
  , recombination_probability_{p}
{}

libbear::population
libbear::stochastic_recombination::
operator()(const genotype& g0, const genotype& g1) const
{
  const auto res_t = recombine_(g0, g1);
  return success(recombination_probability_)
    ? res_t
    : res_t.size() == 2
      ? population{g0, g1}
      : success(.5)
        ? population{g0}
        : population{g1};
}

libbear::variation::
variation(const mutation_fn& m, const recombination_fn& r)
  : mutate_{m}
  , recombine_{r}
{}

libbear::variation::
variation()
  : variation{unary_identity, binary_identity}
{}

libbear::variation::
variation(const mutation_fn& m)
  : variation{m, binary_identity}
{}

libbear::variation::
variation(const recombination_fn& r)
  : variation{unary_identity, r}
{}

libbear::population
libbear::variation::
operator()(const genotype& g0, const genotype& g1) const
{
  population res{};
  for (const auto& g : recombine_(g0, g1)) {
    res.push_back(mutate_(g)[0]);
  }
  assert(res.size() == 1 || res.size() == 2);
  return res;
}

libbear::population
libbear::variation::
operator()(const population& p) const
{
  if (p.size() % 2) {
    throw 42;
  }
  population res;
  for (std::size_t i = 0; i < p.size(); i += 2) {
    for (const auto& g : operator()(p[i], p[i + 1])) {
      res.push_back(g);
    }
  }
  assert(res.size() == p.size() || res.size() == 2 * p.size());
  return res;
}
