#ifndef LIBBEAR_CORE_MEMORY_H
#define LIBBEAR_CORE_MEMORY_H

#include <algorithm>
#include <iterator>

namespace libbear {

  template<template<typename> typename C, typename Ptr>
  C<Ptr> deep_copy(const C<Ptr>& c, const auto& clone) {
    C<Ptr> res{};
    std::transform(std::begin(c), std::end(c), std::back_inserter(res), clone);
    return res;
  }

}

#endif // LIBBEAR_CORE_MEMORY_H

