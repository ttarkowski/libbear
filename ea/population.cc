#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <future>
#include <iterator>
#include <numeric>
#include <thread>
#include <libbear/core/debug.h>
#include <libbear/core/random.h>
#include <libbear/core/thread.h>
#include <libbear/ea/elements.h>
#include <libbear/ea/genotype.h>
#include <libbear/ea/population.h>

namespace {

  libbear::population
  generate(std::size_t lambda, const std::function<libbear::genotype()>& f) {
    libbear::population res{};
    std::generate_n(std::back_inserter(res), lambda, f);
    return res;
  }

}

libbear::populate_2_fn
libbear::adapter(const populate_1_fn& fn) {
  return [=](std::size_t sz, const population& p0, const population& p1) {
    population p{p0};
    p.insert(p.end(), p1.begin(), p1.end());
    return fn(sz, p);
  };
}

libbear::selection_probabilities
libbear::cumulative_probabilities(const selection_probabilities_fn& spf,
                                  const population& p) {
  auto res = spf(p);
  std::partial_sum(res.begin(), res.end(), res.begin());
  // Last element should be exactly equal to 1. and another part of algorithm
  // might require this exact identity. Unfortunately, numerical calculations
  // might not be so precise. Let's check if last element is calculated with
  // 1% precision (basic requirement):
  assert(res.back() > .99 && res.back() < 1.01);
  // Then, let's correct the value:
  res.back() = 1.;
  return res;
}

unsigned int
libbear::random_population::thread_sz = std::thread::hardware_concurrency();

libbear::population
libbear::random_population::
operator()(std::size_t lambda) const {
  // Serial version generated bottleneck for some conditions.
  using type = genotype;
  thread_pool tp{thread_sz};
  std::vector<std::future<type>> v{};
  for (std::size_t i = 0; i < lambda; ++i) {
    v.push_back(tp.async<type>(std::launch::async,
                               [g = g_, this]() mutable -> type {
                                 while(!constraints_(g.random_reset()));
                                 DEBUG_MSG("Random genotype with constraints.");
                                 return g;
                               }));
  }
  population res{};
  for (auto& x : v) {
    res.push_back(x.get());
  }
  return res;
}

libbear::population
libbear::roulette_wheel_selection::
operator()(std::size_t lambda, const population& p) const {
  const auto f =
    [&,
     c = cumulative_probabilities(spf_, p),
     u = random_from_uniform_distribution<double>
     ]() -> genotype {
      return p.at(std::distance(c.begin(),
                                std::lower_bound(c.begin(),
                                                 c.end(),
                                                 u(0., 1.))));
    };
  return generate(lambda, f);
}

libbear::population
libbear::stochastic_universal_sampling::
operator()(std::size_t lambda, const population& p) const {
  const auto a = cumulative_probabilities(spf_, p);
  auto r = random_from_uniform_distribution<double>(0., 1. / lambda);

  population res{};
  for (std::size_t i = 0, j = 0; j < lambda; ++i) {
    for (; r <= a.at(i) && j < lambda; r += 1. / lambda, ++j) {
      res.push_back(p.at(i));
    }
  }
  std::shuffle(res.begin(), res.end(), random_engine());
  return res;
}

libbear::population
libbear::
generational_survivor_selection(std::size_t sz,
                                const population& generation,
                                const population& offspring) {
  if (generation.size() != sz || offspring.size() != sz) {
    throw std::invalid_argument{"generational_survivor_selection: bad size"};
  }
  return offspring;
}

