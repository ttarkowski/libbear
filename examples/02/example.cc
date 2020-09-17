// Evolutionary search for a maximum of given function over domain

#include <cmath>
#include <cstddef>
#include <numbers>
#include <fstream>
#include <libbear/core/range.h>
#include <libbear/ea/elements.h>
#include <libbear/ea/evolution.h>
#include <libbear/ea/fitness.h>
#include <libbear/ea/genotype.h>
#include <libbear/ea/population.h>
#include <libbear/ea/variation.h>

using namespace libbear;

int main() {
  using type = double;

  // function
  const auto f = [](type x, type y) -> type {
    const auto r = [](type x, type y) -> type {
      return std::sqrt(x * x + y * y);
    };
    return std::cos(0.25 * r(x, y)) + std::numbers::e_v<type>;
  };
  // domain
  const range<type> d{-10., +10.};

  const fitness_function ff{
    [&](const genotype& g) {
      return f(g[0]->value<type>(), g[1]->value<type>());
    }
  };

  const auto first_generation_creator =
    random_population{genotype{gene{d}, gene{d}}};
  const auto parents_selection =
    roulette_wheel_selection{fitness_proportional_selection{ff}};
  const auto survivor_selection =
    adapter(roulette_wheel_selection{fitness_proportional_selection{ff}});

  const populate_fns p{first_generation_creator,
                       parents_selection,
                       survivor_selection};

  const type sigma{.2};
  const variation v{Gaussian_mutation<type>{sigma}};
  const std::size_t generation_sz{1000};
  const std::size_t parents_sz{42};
  const generation_creator::options o{v, generation_sz, parents_sz};
  const generation_creator gc{p, o};
  const auto tc = max_iterations_termination(100);
  const evolution e{gc, tc};

  std::ofstream file{"evolution.dat"};
  for (std::size_t i = 0; const auto& x : e()) {
    for (const auto& xx : x) {
      file << i << ' '
           << xx[0]->value<type>() << ' '
           << xx[1]->value<type>() << '\n';
    }
    ++i;
  }
}
