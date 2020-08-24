#ifndef LIBBEAR_EA_GENE_H
#define LIBBEAR_EA_GENE_H

#include <iosfwd>
#include <memory>
#include <stdexcept>
#include "elements.h"
#include "../core/range.h"

namespace libbear {

  class basic_gene {
  public:
    using ptr = std::shared_ptr<basic_gene>;
    
  public:
    virtual ~basic_gene() = default;
    friend std::ostream& operator<<(std::ostream& os, const basic_gene& bg);
    friend bool operator==(const basic_gene& l, const basic_gene& r);
    friend bool operator!=(const basic_gene& l, const basic_gene& r);
    
  protected:
    virtual bool equal(const basic_gene&) const;
    
  private:
    virtual void print(std::ostream&) const = 0;
  };
  
  std::ostream& operator<<(std::ostream&, const basic_gene&);
  bool operator==(const basic_gene&, const basic_gene&);
  bool operator!=(const basic_gene&, const basic_gene&);
    
  template<typename T>
  class gene : public basic_gene {
  public:
    using ptr = std::shared_ptr<gene>;
    
  public:
    explicit gene(T t);
    gene(const gene&) = default;
    T value() const;
    
  protected:
    bool equal(const basic_gene& bg) const override;
    
  private:
    void print(std::ostream& os) const override;
    
  private:
    const T value_;
  };
  
  template<typename T>
  class constrained_gene : public gene<T> {
  public:
    using ptr = typename std::shared_ptr<constrained_gene<T>>;
    
  public:
    constrained_gene(T t, range<T> r);
    range<T> constraints() const;
    
  protected:
    bool equal(const basic_gene& bg) const override;
    
  private:
    const range<T> constraints_;
  };
  
} // namespace libbear

// IMPLEMENTATION

template<typename T>
libbear::gene<T>::gene(T t)
  : value_{t}
{
}

template<typename T>
T libbear::gene<T>::value() const
{
  return value_;
}
  
template<typename T>
bool libbear::gene<T>::equal(const basic_gene& bg) const
{
  const auto g = static_cast<const gene&>(bg);
  return basic_gene::equal(g) && g.value_ == value_;
}

template<typename T> 
void libbear::gene<T>::print(std::ostream& os) const
{
  os << value_;
}

template<typename T>
libbear::constrained_gene<T>::constrained_gene(T t, range<T> r)
  : gene<T>{t}, constraints_{r}
{
  if (!r.contains(t)) {
    throw std::invalid_argument{"constrained_gene: argument is out of range"};
  }
}
  
template<typename T>
libbear::range<T> libbear::constrained_gene<T>::constraints() const
{
  return constraints_;
}

template<typename T>
bool libbear::constrained_gene<T>::equal(const basic_gene& bg) const
{
  const auto cg = static_cast<const constrained_gene&>(bg);
  return gene<T>::equal(cg) && cg.constraints_ == constraints_;
}

#endif // LIBBEAR_EA_GENE_H
