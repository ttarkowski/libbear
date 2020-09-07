#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <libbear/ea/genotype.h>

namespace {

  template<template<typename> typename C, typename T>
  C<std::shared_ptr<T>>
  deep_copy(const C<std::shared_ptr<T>>& c)
  {
    C<std::shared_ptr<T>> res{};
    std::transform(std::begin(c), std::end(c), std::back_inserter(res),
                   [](std::shared_ptr<T> sp) { return sp->clone(); });
    
    return res;
  }
  
}

libbear::genotype::
genotype(const genotype& g)
  : chain_{deep_copy(g.chain_)}
{}

libbear::genotype&
libbear::genotype::
operator=(const genotype& g)
{
  if (&g != this) {
    chain_ = deep_copy(g.chain_);
  }
  return *this;
}

std::size_t
libbear::genotype::
size() const
{
  return chain_.size();
}
    
libbear::genotype::raw_pointer
libbear::genotype::
operator[](std::size_t i) const
{
  return chain_.at(i).get();
}
    
libbear::genotype::value_type&
libbear::genotype::
operator[](std::size_t i)
{
  return chain_.at(i);
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

libbear::genotype::iterator
libbear::genotype::
begin()
{
  return std::begin(chain_);
}

libbear::genotype::iterator
libbear::genotype::
end()
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

libbear::genotype&
libbear::genotype::
random_reset()
{
  for (auto& x : *this) {
    x->random_reset();
  }
  return *this;
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
