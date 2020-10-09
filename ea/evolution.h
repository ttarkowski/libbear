#ifndef LIBBEAR_EVOLUTION_H
#define LIBBEAR_EVOLUTION_H

#include <cstddef>
#include <libbear/ea/elements.h>
#include <libbear/ea/fitness.h>
#include <libbear/ea/variation.h>

namespace libbear {

  class generation_creator {
  public:
    struct options {
      const variation variate;
      const std::size_t generation_sz;
      const std::size_t parents_sz;
    };
    
    generation_creator(const populate_fns& p, const options& o)
      : populate_{p}, options_{o}
    {}

    population operator()() const;
    
  private:
    const populate_fns populate_;
    const options options_;
    mutable bool first_use_{true};
    mutable population current_generation_{};
  };
  
  class evolution {
  public:
    evolution(const generation_creator& gc, const termination_condition& tc)
      : create_generation_{gc}, terminate_{tc}
    {}

    generations operator()() const;
    
  private:
    const generation_creator create_generation_;
    const termination_condition terminate_;
  };
  
  inline termination_condition max_iterations_termination(std::size_t max) {
    return [=](std::size_t i, const generations&) { return i == max; };
  }

  termination_condition
  max_fitness_improvement_termination(const fitness_function& ff,
                                      std::size_t n,
                                      double frac);

}

#endif // LIBBEAR_EVOLUTION_H
