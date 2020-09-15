#pragma once

/// TypeList
template <class... Ts> struct TypeList {};

/// TypeIndex
template <class T, class L> struct TypeIndex;

template <class T> struct TypeIndex<T, TypeList<>> {
  static constexpr int remainder = -1;
  static constexpr int value = -1;
};

template <class T, class U, class... Ts>
struct TypeIndex<T, TypeList<U, Ts...>> {
  static constexpr int remainder = TypeIndex<T, TypeList<Ts...>>::remainder;
  static constexpr int value =
      (remainder == -1 ? -1 : TypeIndex<T, TypeList<Ts...>>::value + 1);
  static_assert(value != -1, "Can not find type in TypeList!");
};

template <class T, class... Ts> struct TypeIndex<T, TypeList<T, Ts...>> {
  static constexpr int remainder = sizeof...(Ts);
  static constexpr int value = 0;
};

/// ConcatList
template <class L1, class L2> struct ConcatList;

template <class... Ts1, class... Ts2>
struct ConcatList<TypeList<Ts1...>, TypeList<Ts2...>> {
  using Type = TypeList<Ts1..., Ts2...>;
};

template <class L1, class L2>
using ConcatListType = typename ConcatList<L1, L2>::Type;

/// ExpandTo
template <class From, class To> using ExpandToType = To;