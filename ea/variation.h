#ifndef LIBBEAR_EA_VARIATION_H
#define LIBBEAR_EA_VARIATION_H

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <tuple>
#include <libbear/core/random.h>
#include <libbear/ea/elements.h>
#include <libbear/ea/genotype.h>

namespace libbear {
  
  inline population unary_identity(const genotype& g) { return population{g}; }

  inline population binary_identity(const genotype& g0, const genotype& g1)
  { return population{g0, g1}; }
  
  class stochastic_mutation {
  public:  
    explicit stochastic_mutation(const mutation_fn& m, probability p)
      : mutate_{m}, mutation_probability_{p}
    {}

    population operator()(const genotype& g) const;
    
  private:
    const mutation_fn mutate_;
    const probability mutation_probability_;
  };
  
  class stochastic_recombination {
  public:
    explicit stochastic_recombination(const recombination_fn& r, probability p)
      : recombine_{r}, recombination_probability_{p}
    {}

    population operator()(const genotype& g0, const genotype& g1) const;
    
  private:
    const recombination_fn recombine_;
    const probability recombination_probability_;
  };
  
  class variation {
  public:
    variation(const mutation_fn& m, const recombination_fn& r)
      : mutate_{m}, recombine_{r}
    {}

    variation()
      : variation{unary_identity, binary_identity}
    {}

    explicit variation(const mutation_fn& m)
      : variation{m, binary_identity}
    {}

    explicit variation(const recombination_fn& r)
      : variation{unary_identity, r}
    {}

    population operator()(const genotype& g0, const genotype& g1) const;
    population operator()(const population& p) const;
    
  private:
    const mutation_fn mutate_;
    const recombination_fn recombine_;
  };
  
  namespace detail {

    template<typename T>
    class iterative_recombination_on_range {
    public:
      using fn = std::function<std::tuple<T, T>(T, T)>;
      using arg_t = std::tuple<fn, std::size_t, std::size_t>;

    public:
      explicit iterative_recombination_on_range(const arg_t& arg)
        : f_{std::get<0>(arg)}
        , start_{std::get<1>(arg)}
        , length_{std::get<2>(arg)}
      {}

      void operator()(genotype& g0, genotype& g1) const {
        if (std::min(g0.size(), g1.size()) < start_ + length_) {
          throw
            std::logic_error{"iterative_recombination_on_range: size mismatch"};
        }
        for (std::size_t i = start_; i < start_ + length_; ++i) {
          const auto [a, b] = f_(g0[i]->value<T>(), g1[i]->value<T>());
          g0[i]->value<T>(a);
          g1[i]->value<T>(b);
        }
      }

    private:
      fn f_;
      std::size_t start_;
      std::size_t length_;
    };

  } // namespace detail

  template<std::size_t N, typename... Ts>
  class iterative_recombination
    : public detail::iterative_recombination_on_range<Ts>... {
    static_assert(N == 1 || N == 2);

    template<typename T>
    using base = detail::iterative_recombination_on_range<T>;

  public:
    template<typename T>
    using fn = std::function<std::tuple<T, T>(T, T)>;

    template<typename T>
    using arg_t = std::tuple<fn<T>, std::size_t, std::size_t>;

  public:
    explicit(sizeof...(Ts) == 1)
    iterative_recombination(const arg_t<Ts>&... args) : base<Ts>{args}...
    {}

    population operator()(const genotype& g0, const genotype& g1) const {
      if (g0.size() != g1.size()) {
        throw std::logic_error{"iterative_recombination: size mismatch"};
      }
      genotype g0_res{g0};
      genotype g1_res{g1};
      (base<Ts>::operator()(g0_res, g1_res), ...);
      if (N == 1) {
        assert(g0_res == g1_res);
        return population{g0_res};
      } else {
        return population{g0_res, g1_res};
      }
    }
  };

  template<typename T>
  population arithmetic_recombination(const genotype& g0, const genotype& g1) {
    using type = typename iterative_recombination<1, T>::arg_t<T>;
    return iterative_recombination<1, T>{
      type{[](T a, T b) {
        const T res = 0.5 * a + 0.5 * b;
        return std::tuple<T, T>{res, res};
      },
      0,
      g0.size()}
    }(g0, g1);
  }

} // namespace libbear

#endif // LIBBEAR_EA_VARIATION_H
