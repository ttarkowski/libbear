#ifndef LIBBEAR_EA_POPULATION_H
#define LIBBEAR_EA_POPULATION_H

#include <functional>
#include <libbear/ea/elements.h>
#include <libbear/ea/genotype.h>

namespace libbear {

  selection_probabilities
  cumulative_probabilities(const selection_probabilities_fn& spf,
                           const population& p);

  class random_population {
  public:
    explicit random_population(const genotype& g)
      : g_{g}
    {}

    population operator()(std::size_t lambda) const;

  private:
    const genotype g_;
  };

  class roulette_wheel_selection {
  public:
    explicit roulette_wheel_selection(const selection_probabilities_fn& spf)
      : spf_{spf}
    {}

    population operator()(std::size_t lambda, const population& p) const;

  private:
    const selection_probabilities_fn  spf_;
  };

  class stochastic_universal_sampling {
  public:
    explicit
    stochastic_universal_sampling(const selection_probabilities_fn& spf)
      : spf_{spf}
    {}

    population operator()(std::size_t lambda, const population& p) const;

  private:
    const selection_probabilities_fn spf_;
  };

  population generational_survivor_selection(std::size_t sz,
                                             const population& generation,
                                             const population& offspring);

} // namespace libbear

#endif // LIBBEAR_EA_POPULATION_H