#ifndef LIBBEAR_CORE_MEMORY_H
#define LIBBEAR_CORE_MEMORY_H

#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

namespace libbear {

  template<template<typename> typename C, typename Ptr>
  C<Ptr> deep_copy(const C<Ptr>& c, const auto& clone) {
    C<Ptr> res{};
    std::ranges::transform(c, std::back_inserter(res), clone);
    return res;
  }

  template<typename T>
  auto make_vector_unique(const auto&... xs) {
    static_assert((std::is_base_of_v<T, std::remove_cvref_t<decltype(xs)>>
                   && ...));
    std::unique_ptr<T> t[] =
      {std::make_unique<std::remove_cvref_t<decltype(xs)>>(xs)...};
    return
      std::vector<std::unique_ptr<T>>{std::make_move_iterator(std::begin(t)),
                                      std::make_move_iterator(std::end(t))};
  }

}

#endif // LIBBEAR_CORE_MEMORY_H

