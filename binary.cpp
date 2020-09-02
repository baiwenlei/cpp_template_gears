#include <iostream>
#include <type_traits>

template <char... chars>
struct Binary {
    static constexpr char buf[] = {(chars+0x30)..., 0};

    static constexpr const char* value() {
        return buf;
    }
};

namespace detail {
    template <class T1, class T2>
    struct BinaryConcat;

    template <char... chars1, char... chars2>
    struct BinaryConcat<Binary<chars1...>, Binary<chars2...>> {
        using type = Binary<chars1..., chars2...>;
    };
}


template <size_t N, class=void>
struct BinaryCreator {
    using type = typename detail::BinaryConcat<typename BinaryCreator<N/2>::type, Binary<N%2>>::type;
};

template <size_t N>
struct BinaryCreator<N, std::enable_if_t<N<2>> {
    using type = Binary<N>;
};


int main() {
    constexpr auto value = BinaryCreator<5>::type::value();
    std::cout << value << "\n";
}
