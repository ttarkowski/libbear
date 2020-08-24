#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include "genotype.h"

std::size_t
libbear::genotype::
size() const
{
  return chain_.size();
}
    
libbear::genotype::value_type
libbear::genotype::
operator[](std::size_t i) const
{
  return chain_[i];
}
    
libbear::genotype::const_iterator
libbear::genotype::
begin() const
{
  return std::begin(chain_);
}

libbear::genotype::const_iterator
libbear::genotype::
end() const
{
  return std::end(chain_);
}

bool
libbear::genotype::
operator==(const genotype& g) const
{
  if (size() != g.size()) {
    return false;
  }
  for (std::size_t i = 0; i < size(); ++i) {
    if (*operator[](i) != *g[i]) {
      return false;
    }
  }
  return true;    
}

bool
libbear::genotype::
operator!=(const genotype& g) const
{
  return !operator==(g);
}

std::ostream&
libbear::
operator<<(std::ostream& os, const genotype& g)
{
  os << "[ ";
  for (const auto& x : g.chain_) {
    os << *x << ' ';
  }
  os << ']';
  return os;
}

std::size_t
std::hash<libbear::genotype>::
operator()(const libbear::genotype& g) const noexcept
{
  // TODO: Better hash function?
  const std::hash<typename libbear::genotype::chain::value_type> h{};
  std::size_t res{0};
  for (const auto& x : g.chain_) {
    res ^= h(x);
  };
  return res;
}
