#include <cstddef>
#include <libbear/ea/elements.h>
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
    res.push_back(create_generation_());
  }
  return res;
}
