#ifndef LIBBEAR_CORE_COORDINATES_H
#define LIBBEAR_CORE_COORDINATES_H

#include <concepts>
#include <cmath>
#include <numbers>
#include <tuple>

namespace libbear {

  namespace detail {

   template<std::floating_point T>
   T angle(T x, T y) {
     const T t = std::atan2(y, x);
     return t >= T{0} ? t : (2 * std::numbers::pi_v<T> + t);
   }

  } // namespace detail

  template<std::floating_point T>
  std::tuple<T, T, T> cart2spher(T x, T y, T z) {
    return std::tuple<T, T, T>{
      std::hypot(x, y, z),                // r
      std::acos(z / std::hypot(x, y, z)), // theta
      detail::angle(x, y)                 // phi
    };
  }

  template<std::floating_point T>
  std::tuple<T, T, T> spher2cart(T r, T theta, T phi) {
    return std::tuple<T, T, T>{
      r * std::sin(theta) * std::cos(phi),
      r * std::sin(theta) * std::sin(phi),
      r * std::cos(theta)
    };
  }

  template<std::floating_point T>
  std::tuple<T, T> cart2polar(T x, T y)
  { return std::tuple<T, T>{std::hypot(x, y), detail::angle(x, y)}; }

  template<std::floating_point T>
  std::tuple<T, T> polar2cart(T r, T phi)
  { return std::tuple<T, T>{r * std::cos(phi), r * std::sin(phi)}; }

} // namespace libbear

#endif // LIBBEAR_CORE_COORDINATES_H
