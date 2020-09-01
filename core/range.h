#ifndef LIBBEAR_CORE_RANGE_H
#define LIBBEAR_CORE_RANGE_H

#include <iostream>
#include <stdexcept>

namespace libbear {
  
  template<typename T>
  class range {
  public:
    range(T min, T max);
    range(const range&) = default;
    range& operator=(const range&) = default;
    T min() const;
    T max() const;
    bool contains(T t) const;
    bool operator==(const range<T>& r) const;
    bool operator!=(const range<T>& r) const;
    
    template<typename U>
    friend std::ostream& operator<<(std::ostream& os, const range<U>& r);
    
  private:
    T min_;
    T max_;
  };

  template<typename T>
  std::ostream& operator<<(std::ostream& os, const range<T>& r);
  
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
T
libbear::range<T>::
min() const
{
  return min_;
}

template<typename T>
T
libbear::range<T>::
max() const
{
  return max_;
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

template<typename T>
bool
libbear::range<T>::
operator!=(const range<T>& r) const
{
  return !operator==(r);
}

template<typename T>
std::ostream&
libbear::operator<<(std::ostream& os, const range<T>& r)
{
  os << '[' << r.min() << ", " << r.max() << ']';
  return os;
}

#endif // LIBBEAR_CORE_RANGE_H
