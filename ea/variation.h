#ifndef LIBBEAR_EA_VARIATION_H
#define LIBBEAR_EA_VARIATION_H

#include "elements.h"
#include "../core/random.h"

namespace libbear {
  
  population unary_identity(const genotype& g);
  population binary_identity(const genotype& g0, const genotype& g1);
  
  class stochastic_mutation {
  public:  
    explicit stochastic_mutation(const mutation_fn& m, probability p);
    population operator()(const genotype& g) const;
    
  private:
    const mutation_fn mutate_;
    const probability mutation_probability_;
  };
  
  class stochastic_recombination {
  public:
    explicit stochastic_recombination(const recombination_fn& r, probability p);
    population operator()(const genotype& g0, const genotype& g1) const;
    
  private:
    const recombination_fn recombine_;
    const probability recombination_probability_;
  };
  
  class variation {
  public:
    variation(const mutation_fn& m, const recombination_fn& r);
    variation();
    explicit variation(const mutation_fn& m);
    explicit variation(const recombination_fn& r);
    population operator()(const genotype& g0, const genotype& g1) const;
    population operator()(const population& p) const;
    
  private:
    const mutation_fn mutate_;
    const recombination_fn recombine_;
  };
  
} // namespace libbear

#endif // LIBBEAR_EA_VARIATION_H
