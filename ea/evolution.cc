#include <cassert>
#include <cstddef>
#include <iterator>
#include <vector>
#include <libbear/core/debug.h>
#include <libbear/ea/elements.h>
#include <libbear/ea/fitness.h>
#include <libbear/ea/evolution.h>
#include <libbear/ea/genotype.h>

libbear::population
libbear::generation_creator::
operator()() const {
  const auto& [p0, p1, p2] = populate_;
  current_generation_ = first_use_
    ? p0(options_.generation_sz)
    : p2(options_.generation_sz,
         current_generation_,
         options_.variate(p1(options_.parents_sz,
                             current_generation_)));
  first_use_ = false;
  return current_generation_;
}
  
libbear::generations
libbear::evolution::
operator()() const {
  generations res{};
  for (std::size_t i = 0; !terminate_(i++, res);) {
    DEBUG_MSG("Generation #" << i);
    res.push_back(create_generation_());
  }
  return res;
}

libbear::termination_condition
libbear::max_fitness_improvement_termination(const fitness_function& ff,
                                             std::size_t n,
                                             double frac) {
  return [=](std::size_t i, const generations& gs) {
    assert(i == gs.size());
    if (gs.size() <= n) {
      return false;
    } else {
      const fitnesses fs{max(gs, ff)};
      const fitness min_last_n = *std::min_element(fs.end() - n, fs.end());
      const double x = (max(fs) - min_last_n) / (max(fs) - min(fs));
      return x <= frac;
    }
  };
}

