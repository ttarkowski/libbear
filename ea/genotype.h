#ifndef LIBBEAR_EA_GENOTYPE_H
#define LIBBEAR_EA_GENOTYPE_H

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <set>
#include <vector>
#include "elements.h"
#include "gene.h"

struct std::hash<libbear::genotype>;

namespace libbear {
  
  class genotype {
    friend std::hash<genotype>;
    
  public:
    using point = std::size_t;
    using crossover_points = std::set<point>;
    using chain = std::vector<basic_gene::ptr>;
    using value_type = typename chain::value_type;
    using const_iterator = typename chain::const_iterator;
    
  public:
    template<typename... Ts> explicit(sizeof...(Ts) == 1) genotype(Ts... ts);
    std::size_t size() const;
    value_type operator[](std::size_t i) const;
    const_iterator begin() const;
    const_iterator end() const;
    bool operator==(const genotype& g) const;
    bool operator!=(const genotype& g) const;
    friend std::ostream& operator<<(std::ostream& os, const genotype& g);
    
  private:
    const chain chain_;  
  };
  
  std::ostream& operator<<(std::ostream&, const genotype&);
  
} // namespace libbear

template<>
struct std::hash<libbear::genotype> {
  std::size_t operator()(const libbear::genotype& g) const noexcept;
};

// IMPLEMENTATION

template<typename... Ts>
libbear::genotype::genotype(Ts... ts)
  : chain_{std::make_shared<gene<const Ts>>(ts)...}
{
}

#endif // LIBBEAR_EA_GENOTYPE_H
