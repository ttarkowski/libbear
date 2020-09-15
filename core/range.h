#ifndef LIBBEAR_CORE_RANGE_H
#define LIBBEAR_CORE_RANGE_H

#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>

namespace libbear {
  
  template<typename T>
  class range {
  public:
    range(T min, T max) : min_{min}, max_{max} {
      if (min > max) {
        throw std::invalid_argument{"range: min greater than max"};
      }
    }

    template<
      typename U = T,
      typename = std::enable_if_t<std::numeric_limits<U>::is_specialized>>
    range()
      : range{std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()}
    {}
    
    range(const range&) = default;
    range(range&&) = default;
    range& operator=(const range&) = default;
    range& operator=(range&&) = default;
    T min() const { return min_; }
    T max() const { return max_; }
    T midpoint const { return std::midpoint(min_, max_); }
    bool contains(T t) const { return t >= min_ && t <= max_; }
    auto operator<=>(const range<T>& r) const = default;
    
  private:
    T min_;
    T max_;
  };

  template<typename T>
  std::ostream& operator<<(std::ostream& os, const range<T>& r)
  { return (os << '[' << r.min() << ", " << r.max() << ']'); }
  
} // namespace libbear

#endif // LIBBEAR_CORE_RANGE_H
