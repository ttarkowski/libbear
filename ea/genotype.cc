#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <iostream>
#include <memory>
#include <numeric>
#include <libbear/core/memory.h>
#include <libbear/ea/genotype.h>

std::size_t libbear::genotype::id_counter = 0;

libbear::genotype&
libbear::genotype::
operator=(const genotype& g) {
  if (&g != this) {
    chain_ = deep_copy(g.chain_, [](const auto& x) { return x->clone(); });
    id_ = g.id_;
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
  // I do not compare id_ fields.
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
  os << "[id: " << g.id() << " genes: ";
  for (const auto& x : g) {
    os << *x << ' ';
  }
  os << ']';
  return os;
}

std::size_t
std::hash<libbear::genotype>::
operator()(const libbear::genotype& g) const noexcept {
  static const std::hash<std::size_t> h{};
  return h(g.id());
}

