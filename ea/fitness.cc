#include <algorithm>
#include <cstddef>
#include <iterator>
#include <future>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>
#include <libbear/core/debug.h>
#include <libbear/core/thread.h>
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
  const auto p = it != fitness_values_->end();
  const auto str = p ? "Fitness taken from database"
                     : "Fitness must be calculated";
  DEBUG_MSG(str);
  return p ? it->second  : ((*fitness_values_)[g] = function_(g));
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
  DEBUG_MSG("Multithreaded calculations: begin");
  using type = std::pair<genotype, fitness>;
  thread_pool tp{thread_sz};
  std::vector<std::future<type>> v{};
  for (const auto& x : uncalculated_fitness(p)) {
    v.push_back(tp.async<type>(std::launch::async, [this, x]() {
                  DEBUG_MSG("Asynchronous fitness calculations");
                  const fitness xf = this->function_(x);
                  return type{x, xf};
                }));
  }
  for (auto& x : v) {
    fitness_values_->insert(x.get());
  }
  DEBUG_MSG("Multithreaded calculations: end");
}

libbear::fitnesses
libbear::select_calculable(const fitnesses& fs, bool require_nonempty_result) {
  fitnesses res{};
  std::ranges::copy_if(fs, std::back_inserter(res),
                       [](fitness f) { return f != incalculable; });
  if (require_nonempty_result && res.size() == 0) {
    throw std::runtime_error{"select_calculable: no calculable fitnesses"};
  }
  return res;
}

libbear::fitness
libbear::
max(const fitnesses& fs) {
  const fitnesses calc{select_calculable(fs, true)};
  return *std::ranges::max_element(calc);
}

libbear::fitness
libbear::
max(const population& p, const fitness_function& ff) {
  return max(ff(p));
}

libbear::fitnesses
libbear::
max(const generations& gs, const fitness_function& ff) {
  fitnesses res{};
  std::ranges::transform(gs, std::back_inserter(res),
                         [&ff](const population& p) { return max(p, ff); });
  return res;
}

libbear::fitness
libbear::
min(const fitnesses& fs) {
  const fitnesses calc{select_calculable(fs, true)};
  return *std::ranges::min_element(calc);
}

libbear::fitness
libbear::
min(const population& p, const fitness_function& ff) {
  return min(ff(p));
}

libbear::fitnesses
libbear::
min(const generations& gs, const fitness_function& ff) {
  fitnesses res{};
  std::ranges::transform(gs, std::back_inserter(res),
                         [&ff](const population& p) { return min(p, ff); });
  return res;
}

libbear::selection_probabilities
libbear::fitness_proportional_selection::
operator()(const population& p) const {
  // FPS with windowing with workarounds for:
  // a) population of equally fit genotypes and
  // b) populations containing genotypes which fitnesses cannot be calculated
  // Please note that in b) case, there should be at least one genotype, which
  // fitness can be calculated.
  DEBUG_MSG("Fitness proportional selection");
  const fitnesses fs{ff_(p)};
  const auto cal = select_calculable(fs, true);
  const fitness min = *std::ranges::min_element(cal);
  const auto n = cal.size();
  const fitness delta = 1. / n;
  const fitness sum =
    std::accumulate(std::begin(cal), std::end(cal), fitness{0.}) - n * min + 1;
  selection_probabilities res{};
  std::ranges::transform(fs, std::back_inserter(res),
                         [=](fitness f) {
                           return
                             f == incalculable? .0 : (f - min + delta) / sum;
                         });
  return res;
}
