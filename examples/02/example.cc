#include <cassert>
#include <cstddef>
#include <libbear/ea/elements.h>
#include <libbear/ea/evolution.h>
#include <libbear/ea/fitness.h>
#include <libbear/ea/population.h>
#include <libbear/ea/variation.h>

int main() {
  using namespace libbear;

  const genotype g{gene{42, 0, 99}, gene{'c', 'a', 'z'}, gene{.42, 0., 1.}};
  const random_population rp{g};

  const fitness_function ff{[](const genotype& g) { return g[2]->value<double>(); }};
  const fitness_proportional_selection fps{ff};
  const roulette_wheel_selection rws{fps};

  const auto& gss = generational_survivor_selection;
  const populate_fns p{rp, rws, gss};

  const variation v{};
  const std::size_t g_sz{42};
  const std::size_t p_sz{42};
  const generation_creator::options o{v, g_sz, p_sz};
  const generation_creator gc{p, o};
  const termination_condition tc =
    [](std::size_t i, const generations&) { return i == 42; };
  const evolution e{gc, tc};
  const generations gs{e()};
  assert(gs.size() == 42);
}
