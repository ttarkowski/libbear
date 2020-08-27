#ifndef LIBBEAR_CORE_RANGE_H
#define LIBBEAR_CORE_RANGE_H

#include <stdexcept>

namespace libbear {
  
  template<typename T>
  class range {
  public:
    range(T min, T max);
    bool contains(T t) const;
    bool operator==(const range<T>& r) const;
    
  private:
    const T min_;
    const T max_;
  };
  
} // namespace libbear

// IMPLEMENTATION

template<typename T>
libbear::range<T>::
range(T min, T max)
  : min_{min}
  , max_{max}
{
  if (min > max) {
    throw std::invalid_argument{"range: min greater than max"};
  }
}

template<typename T>
bool
libbear::range<T>::
contains(T t) const
{
  return t >= min_ && t <= max_;
}

template<typename T>
bool
libbear::range<T>::
operator==(const range<T>& r) const
{
  return min_ == r.min_ && max_ == r.max_;
}

#endif // LIBBEAR_CORE_RANGE_H
