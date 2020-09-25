#include <algorithm>
#include <cstddef>
#include <iterator>
#include <future>
#include <numeric>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>
#include <libbear/ea/elements.h>
#include <libbear/ea/fitness.h>
#include <libbear/ea/genotype.h>

unsigned int
libbear::fitness_function::thread_sz = std::thread::hardware_concurrency();

// TODO: Consider memoization.
libbear::fitness
libbear::fitness_function::
operator()(const genotype& g) const {
  auto it{ fitness_values_->find(g) };
  return it != fitness_values_->end()
    ? it->second
    : ((*fitness_values_)[g] = function_(g));
}

libbear::fitnesses
libbear::fitness_function::
operator()(const population& p) const {
  if (thread_sz > 1 && p.size() > 1) {
    multithreaded_calculations(p);
  }
  fitnesses res{};
  std::ranges::transform(p, std::back_inserter(res),
                         [this](const genotype& g) { return operator()(g); });
  return res;
}

libbear::fitness_function::unique_genotypes
libbear::fitness_function::
uncalculated_fitness(const population& p) const {
  std::unordered_set<genotype> res{};
  std::ranges::copy_if(p, std::inserter(res, std::end(res)),
                       [this](const genotype& g) {
                         return !fitness_values_->contains(g);
                       });
  return res;
}

void
libbear::fitness_function::
multithreaded_calculations(const population& p) const {
  // TODO: Consider thread pool approach or use of HPX library to avoid
  // "starvation" (S in HPX's "SLOW" terminology).
  std::vector<std::future<std::pair<genotype, fitness>>> v{};
  for (std::size_t i = 1; const auto& x : uncalculated_fitness(p)) {
    v.push_back(std::async(std::launch::async, [this, x]() {
                             const fitness xf = this->operator()(x);
                             return std::pair<genotype, fitness>{x, xf};
                           }));
    if (i++ % thread_sz == 0) {
      std::ranges::for_each(v, [](const auto& x) { x.wait(); });
    }
  }
  std::ranges::for_each(v, [](const auto& x) { x.wait(); });
  for (auto& x : v) {
    fitness_values_->insert(x.get());
  }
}

libbear::selection_probabilities
libbear::fitness_proportional_selection::
operator()(const population& p) const {
  // FPS with windowing with workaround for population of equally fit genotypes.
  const fitnesses fs{ff_(p)};
  const fitness min = *std::ranges::min_element(fs);
  const auto n = p.size();
  const fitness delta = 1. / n;
  const fitness sum =
    std::accumulate(std::begin(fs), std::end(fs), fitness{0.}) - n * min + 1;
  selection_probabilities res{};
  std::ranges::transform(fs, std::back_inserter(res),
                         [=](fitness f) { return (f - min + delta) / sum; });
  return res;
}
