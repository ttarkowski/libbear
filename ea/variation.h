#ifndef LIBBEAR_EA_VARIATION_H
#define LIBBEAR_EA_VARIATION_H

#include <libbear/core/random.h>
#include <libbear/ea/elements.h>
#include <libbear/ea/genotype.h>

namespace libbear {
  
  inline population unary_identity(const genotype& g) { return population{g}; }

  inline population binary_identity(const genotype& g0, const genotype& g1)
  { return population{g0, g1}; }
  
  class stochastic_mutation {
  public:  
    explicit stochastic_mutation(const mutation_fn& m, probability p)
      : mutate_{m}, mutation_probability_{p}
    {}

    population operator()(const genotype& g) const;
    
  private:
    const mutation_fn mutate_;
    const probability mutation_probability_;
  };
  
  class stochastic_recombination {
  public:
    explicit stochastic_recombination(const recombination_fn& r, probability p)
      : recombine_{r}, recombination_probability_{p}
    {}

    population operator()(const genotype& g0, const genotype& g1) const;
    
  private:
    const recombination_fn recombine_;
    const probability recombination_probability_;
  };
  
  class variation {
  public:
    variation(const mutation_fn& m, const recombination_fn& r)
      : mutate_{m}, recombine_{r}
    {}

    variation()
      : variation{unary_identity, binary_identity}
    {}

    explicit variation(const mutation_fn& m)
      : variation{m, binary_identity}
    {}

    explicit variation(const recombination_fn& r)
      : variation{unary_identity, r}
    {}

    population operator()(const genotype& g0, const genotype& g1) const;
    population operator()(const population& p) const;
    
  private:
    const mutation_fn mutate_;
    const recombination_fn recombine_;
  };
  
} // namespace libbear

#endif // LIBBEAR_EA_VARIATION_H
