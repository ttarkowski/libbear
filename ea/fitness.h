#ifndef LIBBEAR_EA_FITNESS_H
#define LIBBEAR_EA_FITNESS_H

#include <cstddef>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <unordered_set>
#include <libbear/ea/elements.h>
#include <libbear/ea/genotype.h>

namespace libbear {

  const fitness incalculable = -std::numeric_limits<fitness>::infinity();

  // Fitness function adapted for time consuming fitness value calculations.
  class fitness_function {
  private:
    using unique_genotypes = std::unordered_set<genotype>;

  public:
    using function = std::function<fitness(const genotype&)>;
    static unsigned int thread_sz;
  
  public:
    explicit fitness_function(const function& f)
      : function_{f}
    {}

    fitness_function(const fitness_function&) = default;
    fitness_function& operator=(const fitness_function&) = default;
    fitness operator()(const genotype& g) const;
    fitnesses operator()(const population& p) const;
    std::size_t size() const { return fitness_values_->size(); }

  private:
    unique_genotypes uncalculated_fitness(const population& p) const;
    void multithreaded_calculations(const population& p) const;

  private:
    function function_;
    std::shared_ptr<std::unordered_map<genotype, fitness>> fitness_values_ =
      std::make_shared<std::unordered_map<genotype, fitness>>();
  };
  
  fitnesses select_calculable(const fitnesses& fs,
                              bool require_nonempty_result = false);

  class fitness_proportional_selection {
  public:
    explicit fitness_proportional_selection(const fitness_function& ff)
      : ff_{ff}
    {}

    selection_probabilities operator()(const population& p) const;
    
  private:
    const fitness_function ff_;
  };
  
} // namespace libbear

#endif // LIBBEAR_EA_FITNESS_H
