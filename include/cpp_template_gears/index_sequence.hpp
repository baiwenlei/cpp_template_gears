#pragma once

namespace cpp_template_gears {

template <size_t... I>
struct index_sequence {};

namespace detail {
template <size_t N, size_t... Nexts>
struct make_index_sequence : public make_index_sequence<N-1U, N-1U, Nexts...> {};

template <size_t... Nexts>
struct make_index_sequence<0U, Nexts...> {
    using type = index_sequence<Nexts...>;
};
} // namespace detail

template <size_t N>
using make_index_sequence = typename detail::make_index_sequence<N>::type;

}
