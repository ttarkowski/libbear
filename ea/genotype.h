#ifndef LIBBEAR_EA_GENOTYPE_H
#define LIBBEAR_EA_GENOTYPE_H

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <set>
#include <vector>
#include <libbear/ea/elements.h>
#include <libbear/ea/gene.h>

namespace libbear {
  
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
