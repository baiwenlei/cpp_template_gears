#pragma once

#include <type_traits>

template <char... chars> struct Binary {
  static constexpr char buf[] = {(chars + 0x30)..., 0};

  static constexpr const char *value() { return buf; }
};

template <char... chars> constexpr char Binary<chars...>::buf[];

namespace detail {
template <class T1, class T2> struct BinaryConcat;

template <char... chars1, char... chars2>
struct BinaryConcat<Binary<chars1...>, Binary<chars2...>> {
  using Type = Binary<chars1..., chars2...>;
};
} // namespace detail

template <size_t N, class = void> struct BinaryCreator {
  using Type =
      typename detail::BinaryConcat<typename BinaryCreator<N / 2>::Type,
                                    Binary<N % 2>>::Type;
};

template <size_t N>
    struct BinaryCreator < N,
    typename std::enable_if<N<2>::type> {
  using Type = Binary<N>;
};
