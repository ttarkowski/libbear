#ifndef LIBBEAR_EA_FITNESS_H
#define LIBBEAR_EA_FITNESS_H

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <libbear/ea/elements.h>

namespace libbear {

  // Fitness function adapted for time consuming fitness value calculations.
  class fitness_function {
  public:
    using function = std::function<fitness(const genotype&)>;
  
  public:
    explicit fitness_function(const function& f);
    fitness_function(const fitness_function&) = default;
    fitness_function& operator=(const fitness_function&) = default;
    fitness operator()(const genotype& g) const;
    fitnesses operator()(const population& p) const;
    std::size_t size() const;
    
  private:
    function function_;
    std::shared_ptr<std::unordered_map<genotype, fitness>> fitness_values_ =
      std::make_shared<std::unordered_map<genotype, fitness>>();
  };
  
  class fitness_proportional_selection {
  public:
    explicit fitness_proportional_selection(const fitness_function& ff);
    selection_probabilities operator()(const population& p) const;
    
  private:
    const fitness_function ff_;
  };
  
} // namespace libbear

#endif // LIBBEAR_EA_FITNESS_H
