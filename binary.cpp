#include <iostream>

namespace binary {
    
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


template <size_t N>
struct BinaryCreator {
    using type = typename detail::BinaryConcat<typename BinaryCreator<N/2>::type, Binary<N%2>>::type;
};

template <>
struct BinaryCreator<1> {
    using type = Binary<1>;
};

template <>
struct BinaryCreator<0> {
    using type = Binary<0>;
};

}

int main() {
    std::cout << binary::BinaryCreator<5>::type::value() << "\n";
}
