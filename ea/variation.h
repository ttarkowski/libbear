#ifndef LIBBEAR_EA_VARIATION_H
#define LIBBEAR_EA_VARIATION_H

#include <algorithm>
#include <cassert>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <type_traits>
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
    class iterative_mutation_on_range {
    public:
      using fn = std::function<std::tuple<gene<T>>(const gene<T>&)>;
      using arg_t = std::tuple<fn, std::size_t, std::size_t>;

    public:
      explicit iterative_mutation_on_range(const arg_t& arg)
        : f_{std::get<0>(arg)}
        , start_{std::get<1>(arg)}
        , length_{std::get<2>(arg)}
      {}

      void operator()(const genotype& g) const {
        if (g.size() < start_ + length_) {
          throw
            std::logic_error{"iterative_mutation_on_range: size mismatch"};
        }
        for (std::size_t i = start_; i < start_ + length_; ++i) {
          const auto [a] = f_(*static_cast<gene<T>*>(g[i]));
          *static_cast<gene<T>*>(g[i]) = a;
        }
      }

    private:
      fn f_;
      std::size_t start_;
      std::size_t length_;
    };

    template<typename T>
    class iterative_recombination_on_range {
    public:
      using fn = std::function<std::tuple<gene<T>, gene<T>>(const gene<T>&,
                                                            const gene<T>&)>;
      using arg_t = std::tuple<fn, std::size_t, std::size_t>;

    public:
      explicit iterative_recombination_on_range(const arg_t& arg)
        : f_{std::get<0>(arg)}
        , start_{std::get<1>(arg)}
        , length_{std::get<2>(arg)}
      {}

      void operator()(const genotype& g0, const genotype& g1) const {
        if (std::min(g0.size(), g1.size()) < start_ + length_) {
          throw
            std::logic_error{"iterative_recombination_on_range: size mismatch"};
        }
        for (std::size_t i = start_; i < start_ + length_; ++i) {
          const auto [a, b] = f_(*static_cast<gene<T>*>(g0[i]),
                                 *static_cast<gene<T>*>(g1[i]));
          *static_cast<gene<T>*>(g0[i]) = a;
          *static_cast<gene<T>*>(g1[i]) = b;
        }
      }

    private:
      fn f_;
      std::size_t start_;
      std::size_t length_;
    };

  } // namespace detail

  template<typename... Ts>
  class iterative_mutation : public detail::iterative_mutation_on_range<Ts>... {
    template<typename T>
    using base = detail::iterative_mutation_on_range<T>;

  public:
    template<typename T>
    using fn = std::function<std::tuple<gene<T>>(const gene<T>&)>;

    template<typename T>
    using arg_t = std::tuple<fn<T>, std::size_t, std::size_t>;

  public:
    explicit(sizeof...(Ts) == 1)
    iterative_mutation(const arg_t<Ts>&... args) : base<Ts>{args}...
    {}

    population operator()(const genotype& g) const {
      genotype g_res{g};
      (base<Ts>::operator()(g_res), ...);
      return population{g_res};
    }
  };

  template<std::size_t N, typename... Ts>
  class iterative_recombination
    : public detail::iterative_recombination_on_range<Ts>... {
    static_assert(N == 1 || N == 2);

    template<typename T>
    using base = detail::iterative_recombination_on_range<T>;

  public:
    template<typename T>
    using fn = std::function<std::tuple<gene<T>, gene<T>>(const gene<T>&,
                                                          const gene<T>&)>;

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
      assert(N != 1 || g0_res == g1_res);
      return N == 1? population{g0_res} : population{g0_res, g1_res};
    }
  };

  template<typename T>
  population arithmetic_recombination(const genotype& g0, const genotype& g1) {
    using type = typename iterative_recombination<1, T>::arg_t<T>;
    return iterative_recombination<1, T>{
      type{[](const gene<T>& a, const gene<T>& b) {
        if (a.constraints() != b.constraints()) {
          throw std::logic_error{"arithmetic_recombination: bad constraints"};
        }
        const gene<T> res{std::midpoint(a.value(), b.value()), a.constraints()};
        return std::tuple<gene<T>, gene<T>>{res, res};
      },
      0,
      g0.size()}
    }(g0, g1);
  }

  template<typename T>
  class Gaussian_mutation {
    static_assert(std::is_floating_point_v<T>);

  public:
    explicit Gaussian_mutation(T sigma) : sigma_{sigma} {}

    population operator()(const genotype& g) {
      using type = typename iterative_mutation<T>::arg_t<T>;
      return iterative_mutation<T>{
        type{[this, n = random_from_normal_distribution<T>](const gene<T>& a) {
          const T res = a.constraints().clamp()(a.value() + sigma_ * n(0., 1.));
          return std::tuple<gene<T>>{gene<T>{res, a.constraints()}};
        },
        0,
        g.size()}
      }(g);
    }

  private:
    T sigma_;
  };

} // namespace libbear

#endif // LIBBEAR_EA_VARIATION_H
