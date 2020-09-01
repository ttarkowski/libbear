#ifndef LIBBEAR_EA_GENOTYPE_H
#define LIBBEAR_EA_GENOTYPE_H

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <set>
#include <vector>
#include <libbear/ea/elements.h>
#include <libbear/ea/gene.h>

struct std::hash<libbear::genotype>;

namespace libbear {
  
  class genotype {
    friend std::hash<genotype>;
    
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
    template<typename... Ts> explicit(sizeof...(Ts) == 1) genotype(Ts... ts);
    template<typename... Ts> explicit(sizeof...(Ts) == 1) genotype(std::shared_ptr<Ts>... sps);
    genotype(const genotype& g);
    genotype& operator=(const genotype& g);
    std::size_t size() const;
    raw_pointer operator[](std::size_t i) const;
    value_type& operator[](std::size_t i);
    const_iterator begin() const;
    const_iterator end() const;
    iterator begin();
    iterator end();
    bool operator==(const genotype& g) const;
    bool operator!=(const genotype& g) const;
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

// IMPLEMENTATION

template<typename... Ts>
libbear::genotype::
genotype(Ts... ts)
  : chain_{std::make_shared<gene<Ts>>(ts)...}
{}

template<typename... Ts>
libbear::genotype::
genotype(std::shared_ptr<Ts>... sps)
  : chain_{sps...}
{}

#endif // LIBBEAR_EA_GENOTYPE_H
