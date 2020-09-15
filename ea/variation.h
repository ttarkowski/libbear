#ifndef LIBBEAR_EA_VARIATION_H
#define LIBBEAR_EA_VARIATION_H

#include <stdexcept>
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
  
  template<typename T>
  class iterative_recombination {
    using fn = std::function<T(T, T)>;

  public:
    explicit iterative_recombination(const fn& f) : f_{f} {}

    population operator()(const genotype& g0, const genotype& g1) const {
      if (g0.size() != g1.size()) {
        throw std::logic_error{"iterative_recombination: size mismatch"};
      }
      genotype g_res{g0};
      for (std::size_t i = 0; i < g0.size(); ++i) {
        g_res[i]->value<T>(f_(g0[i]->value<T>(), g1[i]->value<T>()));
      }
      return population{g_res};
    }

  private:
    fn f_;
  };

  template<typename T>
  population arithmetic_recombination(const genotype& g0, const genotype& g1) {
    return iterative_recombination<T>{
      [](T a, T b) { return 0.5 * a + 0.5 * b; }
    }(g0, g1);
  }

} // namespace libbear

#endif // LIBBEAR_EA_VARIATION_H
