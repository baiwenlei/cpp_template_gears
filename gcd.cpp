#include <iostream>

template <size_t M, size_t N>
struct GCD {
    static constexpr size_t value = GCD<N, M%N>::value;
};

template <size_t M>
struct GCD<M, 0> {
    static constexpr size_t value = M;
};


int main() {
    constexpr auto value = GCD<2, 8>::value;
    std::cout << value << "\n";
}
