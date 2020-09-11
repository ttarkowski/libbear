#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <iostream>
#include <memory>
#include <libbear/ea/genotype.h>

namespace {

  template<template<typename> typename C, typename T>
  C<std::unique_ptr<T>>
  deep_copy(const C<std::unique_ptr<T>>& c) {
    C<std::unique_ptr<T>> res{};
    for (auto it = c.begin(); it != c.end(); ++it) {
      res.push_back((**it).clone());
    }
    return res;
  }
  
}

libbear::genotype::
genotype(const genotype& g)
  : chain_{deep_copy(g.chain_)}
{}

libbear::genotype&
libbear::genotype::
operator=(const genotype& g) {
  if (&g != this) {
    chain_ = deep_copy(g.chain_);
  }
  return *this;
}

bool
libbear::genotype::
operator==(const genotype& g) const {
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

libbear::genotype&
libbear::genotype::
random_reset() {
  for (auto& x : *this) {
    x->random_reset();
  }
  return *this;
}

std::ostream&
libbear::
operator<<(std::ostream& os, const genotype& g) {
  os << "[ ";
  for (const auto& x : g.chain_) {
    os << *x << ' ';
  }
  os << ']';
  return os;
}

std::size_t
std::hash<libbear::genotype>::
operator()(const libbear::genotype& g) const noexcept {
  // TODO: Better hash function?
  const std::hash<typename libbear::genotype::chain::value_type> h{};
  std::size_t res{0};
  for (const auto& x : g) {
    res ^= h(x);
  };
  return res;
}

