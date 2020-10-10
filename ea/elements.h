#ifndef LIBBEAR_EA_ELEMENTS
#define LIBBEAR_EA_ELEMENTS

#include <functional>
#include <vector>
#include <libbear/core/random.h>

namespace libbear {

  class genotype;
  
  using population = std::vector<genotype>;
  using fitness = double;
  using fitnesses = std::vector<fitness>;
  
  // Mapping:
  template<typename P> using encoding_fn = std::function<genotype(const P&)>;
  template<typename P> using decoding_fn = std::function<P(const genotype&)>;
  
  using selection_probabilities =
    std::vector<probability>;
  using selection_probabilities_fn =
    std::function<selection_probabilities(const population&)>;

  using mutation_fn =
    std::function<population(const genotype&)>;
  using recombination_fn =
    std::function<population(const genotype&, const genotype&)>;
  
  // Population generators/selectors
  // - first generation creator
  using populate_0_fn =
    std::function<population(std::size_t)>;
  // - parents selection
  using populate_1_fn =
    std::function<population(std::size_t, population)>;
  // - survivor selection
  using populate_2_fn =
    std::function<population(std::size_t, population, population)>;
  
  using populate_fns =
    std::tuple<populate_0_fn, populate_1_fn, populate_2_fn>;
  
  using generations =
    std::vector<population>;
  using termination_condition =
    std::function<bool(std::size_t, const generations&)>;
  
  using genotype_constraints = std::function<bool(const genotype&)>;

} // namespace libbear

#endif // LIBBEAR_EA_ELEMENTS
