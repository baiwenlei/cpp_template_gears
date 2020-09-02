#include <iostream>
#include <type_traits>

template <char... chars>
struct NBaseSeq {
    static constexpr char buf[] = {(chars+0x30)..., 0};

    static constexpr const char* value() {
        return buf;
    }
};

namespace detail {
    template <class T1, class T2>
    struct NBaseSeqConcat;

    template <char... chars1, char... chars2>
    struct NBaseSeqConcat<NBaseSeq<chars1...>, NBaseSeq<chars2...>> {
        using type = NBaseSeq<chars1..., chars2...>;
    };
}


template <size_t N, size_t B, class=void>
struct NBaseSeqCreator {
    using type = typename detail::NBaseSeqConcat<typename NBaseSeqCreator<N/B, B>::type, NBaseSeq<N%B>>::type;
};

template <size_t N, size_t B>
struct NBaseSeqCreator<N, B, std::enable_if_t<N<B>> {
    using type = NBaseSeq<N>;
};


int main() {
    constexpr auto value = NBaseSeqCreator<16, 8>::type::value();
    std::cout << value << "\n";
}
