// Evolutionary search for the best crystal structure of boron nanostripe
// - function: self consistent field calculations with Quantum ESPRESSO
// - domain: [.25, pi] x [0.5, 2.]
// - variation type: Gaussian mutation and arithmetic recombination

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <mutex>
#include <numbers>
#include <fstream>
#include <limits>
#include <sstream>
#include <type_traits>
#include <libbear/core/range.h>
#include <libbear/core/system.h>
#include <libbear/ea/elements.h>
#include <libbear/ea/evolution.h>
#include <libbear/ea/fitness.h>
#include <libbear/ea/genotype.h>
#include <libbear/ea/population.h>
#include <libbear/ea/variation.h>

using namespace libbear;

namespace {

  const std::string pp{"B.pbesol-n-kjpaw_psl.0.1.UPF"};

  template<typename T>
  void input_file(const std::string& filename, T distance, T angle) {
    static_assert(std::is_floating_point_v<T>);
    std::ofstream file{filename};
    T dx = distance * std::sin(angle / 2.);
    T dy = distance * std::cos(angle / 2.);
    const std::string z{"000000000"};
    file << "&CONTROL\ncalculation = 'scf'\nprefix = 'dft'\npseudo_dir = './'\n"
         << "outdir = 'results-" << filename << "'\n/\n&SYSTEM\nibrav = 0\n"
         << "nat = 2\nntyp = 1\ntot_charge = 0." << z << "D+00\n"
         << "occupations = 'smearing'\nsmearing = 'methfessel-paxton'\n"
         << "degauss = 5." << z << "D-03\n" << "ecutwfc = 6." << z << "D+01\n"
         << "/\n&ELECTRONS\nelectron_maxstep = 100000\nmixing_beta = 7." << z
         << "D-01\n/\nCELL_PARAMETERS angstrom\n"
         << std::fixed << std::setprecision(9) << 2 * dx << " 0." << z << " 0."
         << z << "\n0." << z << " " << std::fixed << std::setprecision(9)
         << 15 + dy << " 0." << z << "\n0." << z << " 0." << z << " 15." << z
         <<"\n\nATOMIC_SPECIES\nB11 11.009305 " << pp << "\n\n"
         << "ATOMIC_POSITIONS angstrom\nB11 0." << z << " 0." << z << " 0."
         << z << "\nB11 " << std::fixed << std::setprecision(9) << dx << " "
         << std::fixed << std::setprecision(9) << dy << " 0." << z << "\n\n"
         << "K_POINTS automatic\n4 1 1 1 1 0\n"; // 16 1 1 1 1 0
  }

  std::string unique_filename() {
    static std::size_t i{0};
    static std::mutex m{};
    std::lock_guard<std::mutex> g{m};
    std::ostringstream oss{};
    oss << "stripe-" << i++ << ".in";
    return oss.str();
  }

}

int main() {
  using type = double;

  execute("/bin/bash download.sh " + pp);

  // function
  const auto f = [](type distance, type angle) -> type {
    const std::string input_filename{unique_filename()};
    input_file(input_filename, distance, angle);
    const auto [o, e] = execute("/bin/bash calc.sh " + input_filename);
    return o == "Calculations failed.\n"
      ? -std::numeric_limits<type>::infinity()
      : -std::stod(o);
  };
  // domain
  // Min angle can be calculated from this equation:
  // 2 * distance * sin(angle / 2.) >= min bond length == 0.5
  const range<type> angle_range{.25, std::numbers::pi_v<type>}; // rad
  const range<type> bond_range{0.5, 2.}; // Angstrom

  const fitness_function ff{
    [&](const genotype& g) {
      return f(g[0]->value<type>(), g[1]->value<type>());
    }
  };

  const auto first_generation_creator =
    random_population{genotype{gene{angle_range}, gene{bond_range}}};
  const auto parents_selection =
    roulette_wheel_selection{fitness_proportional_selection{ff}};
  const auto survivor_selection =
    adapter(roulette_wheel_selection{fitness_proportional_selection{ff}});

  const populate_fns p{first_generation_creator,
                       parents_selection,
                       survivor_selection};

  const type sigma{.02};
  const variation v{Gaussian_mutation<type>{sigma},
                    arithmetic_recombination<type>};
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
           << xx[1]->value<type>() << ' '
           << ff(xx) << '\n';
    }
    ++i;
  }
}
