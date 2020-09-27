#pragma once
#include <cpp_template_gears/type_list.hpp>
#include <tuple>

/// IndexedTuple
template <class K, class V> struct IndexedTuple;

template <class... Ks, class... Vs>
struct IndexedTuple<TypeList<Ks...>, TypeList<Vs...>>
    : public std::tuple<Vs...> {
  using Base = std::tuple<Vs...>;

  using Base::Base;

  // 想用std::tuple_cat构造必须使能如下行
  template <class... Us>
  IndexedTuple(Us &&... args) : Base(std::forward<Us>(args)...) {}

  template <class T, class U = std::enable_if_t<
                         TypeIndex<T, TypeList<Ks...>>::value != -1>>
  decltype(auto) get() {
    return std::get<TypeIndex<T, TypeList<Ks...>>::value>(*this);
  }

  template <class T, class U = std::enable_if_t<
                         TypeIndex<T, TypeList<Ks...>>::value != -1>>
  decltype(auto) get() const {
    return std::get<TypeIndex<T, TypeList<Ks...>>::value>(*this);
  }

  template <class... K2s, class... V2s>
  auto
  concat(const IndexedTuple<TypeList<K2s...>, TypeList<V2s...>> &tuple2) const {
    return IndexedTuple<TypeList<Ks..., K2s...>, TypeList<Vs..., V2s...>>{
        // std::tuple_cat(static_cast<const Base&>(*this), static_cast<const
        // Base2&>(tuple2))
        get<Ks>()..., tuple2.template get<K2s>()...};
  }
};
