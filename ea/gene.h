#ifndef LIBBEAR_EA_GENE_H
#define LIBBEAR_EA_GENE_H

#include <iosfwd>
#include <limits>
#include <memory>
#include <stdexcept>
#include <libbear/core/random.h>
#include <libbear/core/range.h>
#include <libbear/ea/elements.h>

namespace libbear {

  template<typename T> class gene;
  template<typename T> class constrained_gene;

  namespace detail {

    class basic_gene;
    
    class basic_gene_restrictions {
      friend class libbear::detail::basic_gene;
      // Classes derived from basic_gene have deleted ctors. Only classes listed
      // below are allowed to have them. (This is workaround to the problem of
      // restricting inheriance from basic_gene.)
      template<typename T> friend class libbear::gene;
      template<typename T> friend class libbear::constrained_gene;
      
    private:
      basic_gene_restrictions() = default;
      
    public:
      virtual ~basic_gene_restrictions() = default;
    };

    class basic_gene : virtual basic_gene_restrictions {
    public:
      using ptr = std::shared_ptr<basic_gene>;
      
    public:
      virtual ~basic_gene() = default;
      template<typename T> T value() const;
      template<typename T> gene<T>& value(T t);
      virtual basic_gene& random_reset() = 0;
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
  
  }

  template<typename T>
  class gene : public detail::basic_gene {
  public:
    using ptr = std::shared_ptr<gene>;
    
  public:
    explicit gene(T t);
    gene(const gene&) = default;
    T value() const;
    gene& value(T t);
    gene& random_reset() override;

  protected:
    bool equal(const detail::basic_gene& bg) const override;
    
  private:
    using detail::basic_gene::value;
    void print(std::ostream& os) const override;

  private:
    T value_;
  };
  
  template<typename T>
  class constrained_gene final : public gene<T> {
  public:
    using ptr = typename std::shared_ptr<constrained_gene<T>>;
    
  public:
    constrained_gene(T t, range<T> r);
    range<T> constraints() const;
    constrained_gene& constraints(const range<T>& r);
    constrained_gene& random_reset() override;
    
  protected:
    bool equal(const detail::basic_gene& bg) const override;
    
  private:
    range<T> constraints_;
  };
  
} // namespace libbear

// IMPLEMENTATION

template<typename T>
T
libbear::detail::basic_gene::
value() const
{
  // I can perform downcast, because I have previously limited set of usable
  // classes. For more details please see note for basic_gene_restrictions.
  return static_cast<const gene<T>*>(this)->value();
}

template<typename T>
libbear::gene<T>&
libbear::detail::basic_gene::
value(T t)
{
  return static_cast<gene<T>*>(this)->value(t);
}

template<typename T>
libbear::gene<T>::
gene(T t)
  : value_{t}
{
}

template<typename T>
T
libbear::gene<T>::
value() const
{
  return value_;
}

template<typename T>
libbear::gene<T>&
libbear::gene<T>::
value(T t)
{
  value_ = t;
  return *this;
}

template<typename T>
libbear::gene<T>&
libbear::gene<T>::
random_reset()
{
  value_ = random_from_uniform_distribution<T>(std::numeric_limits<T>::lowest(),
                                               std::numeric_limits<T>::max());
  return *this;
}
  
template<typename T>
bool
libbear::gene<T>::
equal(const detail::basic_gene& bg) const
{
  const auto g = static_cast<const gene&>(bg);
  return detail::basic_gene::equal(g) && g.value_ == value_;
}

template<typename T> 
void
libbear::gene<T>::
print(std::ostream& os) const
{
  os << value_;
}

template<typename T>
libbear::constrained_gene<T>::
constrained_gene(T t, range<T> r)
  : gene<T>{t}, constraints_{r}
{
  if (!r.contains(t)) {
    throw std::invalid_argument{"constrained_gene: argument is out of range"};
  }
}
  
template<typename T>
libbear::range<T>
libbear::constrained_gene<T>::
constraints() const
{
  return constraints_;
}

template<typename T>
libbear::constrained_gene<T>&
libbear::constrained_gene<T>::
constraints(const range<T>& r)
{
  constraints_ = r;
  return *this;
}

template<typename T>
libbear::constrained_gene<T>&
libbear::constrained_gene<T>::
random_reset()
{
  this->value(random_from_uniform_distribution<T>(constraints_.min(),
                                                  constraints_.max()));
  return *this;
}

template<typename T>
bool
libbear::constrained_gene<T>::
equal(const detail::basic_gene& bg) const
{
  const auto cg = static_cast<const constrained_gene&>(bg);
  return gene<T>::equal(cg) && cg.constraints_ == constraints_;
}

#endif // LIBBEAR_EA_GENE_H
