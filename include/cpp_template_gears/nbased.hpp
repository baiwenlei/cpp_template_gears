#pragma once

#include <type_traits>

template <char... chars> struct NBaseSeq {
  static constexpr char buf[] = {(chars + 0x30)..., 0};

  static constexpr const char *value() { return buf; }
};

template <char... chars> constexpr char NBaseSeq<chars...>::buf[];

namespace detail {
template <class T1, class T2> struct NBaseSeqConcat;

template <char... chars1, char... chars2>
struct NBaseSeqConcat<NBaseSeq<chars1...>, NBaseSeq<chars2...>> {
  using Type = NBaseSeq<chars1..., chars2...>;
};
} // namespace detail

template <size_t N, size_t B, class = void> struct NBaseSeqCreator {
  using Type =
      typename detail::NBaseSeqConcat<typename NBaseSeqCreator<N / B, B>::Type,
                                      NBaseSeq<N % B>>::Type;
};

template <size_t N, size_t B>
    struct NBaseSeqCreator < N,
    B, typename std::enable_if<N<B>::type> {
  using Type = NBaseSeq<N>;
};
