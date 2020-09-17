#ifndef LIBBEAR_EA_GENOTYPE_H
#define LIBBEAR_EA_GENOTYPE_H

#include <compare>
#include <cstddef>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>
#include <libbear/core/memory.h>
#include <libbear/core/random.h>
#include <libbear/core/range.h>

namespace libbear {

  template<typename T> class gene;

  namespace detail {

    class basic_gene;
    template<typename T> class typed_gene;

    class basic_gene_restrictions {
      friend class libbear::detail::basic_gene;
      // Classes derived from basic_gene have deleted ctors. Only classes listed
      // below are allowed to have them. (This is workaround to the problem of
      // restricting inheritance from basic_gene.)
      template<typename T> friend class libbear::detail::typed_gene;
      template<typename T> friend class libbear::gene;

    private:
      basic_gene_restrictions() = default;

    public:
      virtual ~basic_gene_restrictions() = default;
    };

    class basic_gene : virtual basic_gene_restrictions {
    public:
      using ptr = std::unique_ptr<basic_gene>;

    public:
      virtual ~basic_gene() = default;
      virtual ptr clone() const = 0;

      template<typename T>
      T value() const {
        // I can perform downcast, because I have previously limited set of
        // usable classes. For more details please see note for
        // basic_gene_restrictions.
        return static_cast<const typed_gene<T>*>(this)->value();
      }

      template<typename T>
      typed_gene<T>& value(T t)
      { return static_cast<typed_gene<T>*>(this)->value(t); }

      template<typename T>
      range<T> constraints() const
      { return static_cast<const gene<T>*>(this)->constraints(); }

      template<typename T>
      gene<T>& constraints(const range<T>& r)
      { return static_cast<gene<T>*>(this)->constraints(r); }

      virtual basic_gene& random_reset() = 0;

      friend std::ostream& operator<<(std::ostream& os, const basic_gene& bg)
      { return bg.print(os); }

      friend bool operator==(const basic_gene& l, const basic_gene& r)
      { return typeid(l) == typeid(r) && l.equal(r); }

      friend std::partial_ordering operator<=>(const basic_gene& l,
                                               const basic_gene& r) {
        if (typeid(l) != typeid(r)) {
          throw std::runtime_error{"operator<=>: different dynamic types"};
        }
        return l.spaceship(r);
      }

    protected:
      virtual bool equal(const basic_gene&) const { return true; }
      virtual std::ostream& print(std::ostream&) const = 0;

    private:
      virtual std::partial_ordering spaceship(const basic_gene&) const {
        return std::partial_ordering::equivalent;
      }
    };

    std::ostream& operator<<(std::ostream&, const basic_gene&);
    bool operator==(const basic_gene&, const basic_gene&);
    std::partial_ordering operator<=>(const basic_gene&, const basic_gene&);

    template<typename T>
    class typed_gene : public basic_gene {
    public:
      using ptr = std::unique_ptr<typed_gene>;
      using base_ptr = typename basic_gene::ptr;

    public:
      explicit typed_gene(T t) : value_{t} {}
      typed_gene(const typed_gene&) = default;
      typed_gene(typed_gene&&) = default;
      typed_gene& operator=(const typed_gene&) = default;
      typed_gene& operator=(typed_gene&&) = default;

      base_ptr clone() const override
      { return std::make_unique<typed_gene>(*this); }

      T value() const { return value_; }

      virtual typed_gene& value(T t) {
        value_ = t;
        return *this;
      }

      typed_gene& random_reset() override {
        value_ =
          random_from_uniform_distribution<T>(std::numeric_limits<T>::lowest(),
                                              std::numeric_limits<T>::max());
        return *this;
      }

    protected:
      std::ostream& print(std::ostream& os) const override
      { return (os << value_); }

    private:
      using basic_gene::value;
      using basic_gene::constraints;

      bool equal(const basic_gene& bg) const override {
        const auto g = static_cast<const typed_gene&>(bg);
        return basic_gene::equal(g) && g.value_ == value_;
      }

      std::partial_ordering spaceship(const basic_gene& bg) const override {
        const auto g = static_cast<const typed_gene&>(bg);
        return g.value_ <=> value_;
      }

    private:
      T value_;
    };
  }

  template<typename T>
  class gene final : public detail::typed_gene<T> {
  public:
    using ptr = typename std::unique_ptr<gene<T>>;
    using base_ptr = typename detail::basic_gene::ptr;

  public:
    gene(T t, const range<T>& r)
      : detail::typed_gene<T>{t}, constraints_{r} {
      if (!r.contains(t)) {
        throw std::invalid_argument{"gene: value is out of range"};
      }
    }

    explicit gene(const range<T>& r) : gene{r.midpoint(), r} {}
    gene(T t, T min, T max) : gene{t, range<T>{min, max}} {}
    gene(T min, T max) : gene{range<T>{min, max}} {}
    gene(const gene&) = default;
    gene(gene&&) = default;
    gene& operator=(const gene&) = default;
    gene& operator=(gene&&) = default;

    base_ptr clone() const override
    { return std::make_unique<gene>(*this); }

    range<T> constraints() const { return constraints_; }

    gene& constraints(const range<T>& r) {
      if (!r.contains(this->value())) {
        throw std::invalid_argument("gene: bad range");
      }
      constraints_ = r;
      return *this;
    }

    // I can't use:
    //   using detail::typed_gene<T>::value;
    // because it conflicts with hidden basic_gene::value. Thus I redefine
    // non-virtual to get around the hiding rule:
    T value() const { return detail::typed_gene<T>::value(); }

    gene& value(T t) override {
      if (!constraints_.contains(t)) {
        throw std::invalid_argument{"gene: bad value"};
      }
      detail::typed_gene<T>::value(t);
      return *this;
    }

    gene& random_reset() override {
      this->value(random_from_uniform_distribution<T>(constraints_.min(),
                                                      constraints_.max()));
      return *this;
    }

  protected:
    std::ostream& print(std::ostream& os) const override
    { return (detail::typed_gene<T>::print(os) << " in " << constraints_); }

  private:
    range<T> constraints_;
  };
  
  class genotype {
  public:
    using point = std::size_t;
    using crossover_points = std::set<point>;
    using chain = std::vector<detail::basic_gene::ptr>;
    using const_iterator = typename chain::const_iterator;
    using iterator = typename chain::iterator;
    using raw_pointer = detail::basic_gene*;
    
  public:
    genotype() = default;

    template<typename... Ts>
    explicit(sizeof...(Ts) == 1) genotype(const gene<Ts>&... gs)
      : chain_{make_vector_unique<detail::basic_gene>(gs...)}
    {}

    genotype(const genotype& g)
      : chain_{deep_copy(g.chain_, [](const auto& x) { return x->clone(); })}
    {}

    genotype& operator=(const genotype& g);
    std::size_t size() const { return chain_.size(); }
    raw_pointer operator[](std::size_t i) const { return chain_[i].get(); }
    raw_pointer at(std::size_t i) const { return chain_.at(i).get(); }
    const_iterator begin() const { return chain_.begin(); }
    const_iterator end() const { return chain_.end(); }
    iterator begin() { return chain_.begin(); }
    iterator end() { return chain_.end(); }
    bool operator==(const genotype& g) const;
    genotype& random_reset();
    friend std::ostream& operator<<(std::ostream& os, const genotype& g);
    
  private:
    chain chain_;
  };
  
  std::ostream& operator<<(std::ostream&, const genotype&);
  
} // namespace libbear

template<>
struct std::hash<libbear::genotype> {
  std::size_t operator()(const libbear::genotype& g) const noexcept;
};

#endif // LIBBEAR_EA_GENOTYPE_H
