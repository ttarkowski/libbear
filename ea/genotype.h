#ifndef LIBBEAR_EA_GENOTYPE_H
#define LIBBEAR_EA_GENOTYPE_H

#include <cstddef>
#include <iosfwd>
#include <limits>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>
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
      virtual ptr clone() const = 0;

      template<typename T>
      T value() const {
        // I can perform downcast, because I have previously limited set of
        // usable classes. For more details please see note for
        // basic_gene_restrictions.
        return static_cast<const gene<T>*>(this)->value();
      }

      template<typename T>
      gene<T>& value(T t)
      { return static_cast<gene<T>*>(this)->value(t); }

      virtual basic_gene& random_reset() = 0;

      friend std::ostream& operator<<(std::ostream& os, const basic_gene& bg)
      { return bg.print(os); }

      friend bool operator==(const basic_gene& l, const basic_gene& r)
      { return typeid(l) == typeid(r) && l.equal(r); }

    protected:
      virtual bool equal(const basic_gene&) const { return true; }
      virtual std::ostream& print(std::ostream&) const = 0;
    };

    std::ostream& operator<<(std::ostream&, const basic_gene&);
    bool operator==(const basic_gene&, const basic_gene&);

  }

  template<typename T>
  class gene : public detail::basic_gene {
  public:
    using ptr = std::shared_ptr<gene>;
    using base_ptr = typename detail::basic_gene::ptr;

  public:
    explicit gene(T t) : value_{t} {}
    gene(const gene&) = default;
    gene& operator=(const gene&) = default;
    base_ptr clone() const override { return std::make_shared<gene>(*this); }
    T value() const { return value_; }

    gene& value(T t) {
      value_ = t;
      return *this;
    }

    gene& random_reset() override {
      value_ =
        random_from_uniform_distribution<T>(std::numeric_limits<T>::lowest(),
                                            std::numeric_limits<T>::max());
      return *this;
    }

  protected:
    bool equal(const detail::basic_gene& bg) const override {
      const auto g = static_cast<const gene&>(bg);
      return detail::basic_gene::equal(g) && g.value_ == value_;
    }

    std::ostream& print(std::ostream& os) const override
    { return (os << value_); }

  private:
    using detail::basic_gene::value;

  private:
    T value_;
  };

  template<typename T>
  class constrained_gene final : public gene<T> {
  public:
    using ptr = typename std::shared_ptr<constrained_gene<T>>;
    using base_ptr = typename detail::basic_gene::ptr;

  public:
    constrained_gene(T t, range<T> r) : gene<T>{t}, constraints_{r} {
      if (!r.contains(t)) {
        throw
          std::invalid_argument{"constrained_gene: argument is out of range"};
      }
    }

    constrained_gene(const constrained_gene&) = default;
    constrained_gene& operator=(const constrained_gene&) = default;

    base_ptr clone() const override
    { return std::make_shared<constrained_gene>(*this); }

    range<T> constraints() const { return constraints_; }

    constrained_gene& constraints(const range<T>& r) {
      if (!r.contains(this->value())) {
        throw std::invalid_argument("constrained_gene: bad range");
      }
      constraints_ = r;
      return *this;
    }

    constrained_gene& random_reset() override {
      this->value(random_from_uniform_distribution<T>(constraints_.min(),
                                                      constraints_.max()));
      return *this;
    }

  protected:
    bool equal(const detail::basic_gene& bg) const override {
      const auto cg = static_cast<const constrained_gene&>(bg);
      return gene<T>::equal(cg) && cg.constraints_ == constraints_;
    }

    std::ostream& print(std::ostream& os) const override
    { return (gene<T>::print(os) << " in " << constraints_); }

  private:
    range<T> constraints_;
  };
  
  class genotype {
  public:
    using point = std::size_t;
    using crossover_points = std::set<point>;
    using chain = std::vector<detail::basic_gene::ptr>;
    using value_type = typename chain::value_type;
    using const_iterator = typename chain::const_iterator;
    using iterator = typename chain::iterator;
    using raw_pointer = detail::basic_gene*;
    
  public:
    genotype() = default;

    // Note: genotype ctor was previously conditionally explicit:
    //   explicit(sizeof...(Ts) == 1)
    // That solution leads to greedy and unintentional construction of genotype
    // objects. The genotype ctor should be explicit without any conditions:
    template<typename... Ts>
    explicit genotype(Ts... ts)
      : chain_{std::make_shared<gene<Ts>>(ts)...}
    {}
    
    template<typename... Ts>
    explicit genotype(std::shared_ptr<Ts>... sps)
      : chain_{sps...}
    {}
    
    genotype(const genotype& g);
    genotype& operator=(const genotype& g);
    std::size_t size() const { return chain_.size(); }
    raw_pointer operator[](std::size_t i) const { return chain_[i].get(); }
    value_type& operator[](std::size_t i) { return chain_[i]; }
    raw_pointer at(std::size_t i) const { return chain_.at(i).get(); }
    value_type& at(std::size_t i) { return chain_.at(i); }
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
