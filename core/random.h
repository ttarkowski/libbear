#ifndef LIBBEAR_CORE_RANDOM_H
#define LIBBEAR_CORE_RANDOM_H

#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <type_traits>

namespace libbear {

  using probability = double;

  std::mt19937& random_engine();

  inline bool success(probability success_probability)
  { return std::bernoulli_distribution{ success_probability }(random_engine()); }

  template<typename T>
  T random_from_normal_distribution(T mean, T standard_deviation)
  {
    static_assert(std::is_floating_point_v<T>);
    auto& generator{ random_engine() };
    return std::normal_distribution<T>{ mean, standard_deviation }(generator);
  }
  
  template<typename T>
  T random_from_uniform_distribution(T a, T b)
  {
    auto& generator{ random_engine() };
    if constexpr (std::is_floating_point_v<T>) {
      assert(a < b);
      return // Check whether b - a overflows [N4861, 26.6.8.2.2].
        a > 0. || b <= std::numeric_limits<T>::max() + a
          ? std::uniform_real_distribution<T>{ a, b }(generator) // [a, b)
          : random_from_uniform_distribution(false, true)
            ? random_from_uniform_distribution<T>(a, std::midpoint(a, b))
            : random_from_uniform_distribution<T>(std::midpoint(a, b), b);
    } else if constexpr (std::is_same_v<T, bool>) {
      return a == b ? a : std::bernoulli_distribution{ 0.5 }(generator);
    } else { // [a, b]
      assert(a <= b);
      return std::uniform_int_distribution<T>{ a, b }(generator);
    }
  }
  
  template<typename>
  class range;
  
  template<typename T>
  T random(const range<T>& r)
  {
    return random_from_uniform_distribution<T>(r.min(), r.max());
  }

} // namespace libbear

#endif // LIBBEAR_CORE_RANDOM_H
