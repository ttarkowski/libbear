#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <iostream>
#include <memory>
#include <numeric>
#include <libbear/core/memory.h>
#include <libbear/ea/genotype.h>

libbear::genotype&
libbear::genotype::
operator=(const genotype& g) {
  if (&g != this) {
    chain_ = deep_copy(g.chain_, [](const auto& x) { return x->clone(); });
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
  for (const auto& x : *this) {
    x->random_reset();
  }
  return *this;
}

std::ostream&
libbear::
operator<<(std::ostream& os, const genotype& g) {
  os << "[ ";
  for (const auto& x : g) {
    os << *x << ' ';
  }
  os << ']';
  return os;
}

std::size_t
std::hash<libbear::genotype>::
operator()(const libbear::genotype& g) const noexcept {
  const std::hash<typename libbear::genotype::chain::value_type> h{};
  return std::accumulate(std::begin(g), std::end(g), std::size_t{0},
                         [&h](auto acc, const auto& x) { return acc ^= h(x); });
}

