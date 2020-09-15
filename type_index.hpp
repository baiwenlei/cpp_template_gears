#include <iostream>

template <class... Ts>
struct TypeList{};

template <class T, class L>
struct TypeIndex;

template <class T>
struct TypeIndex<T, TypeList<>> {
    static constexpr int remainder = -1;
    static constexpr int value = -1;
};

template <class T, class U, class... Ts>
struct TypeIndex<T, TypeList<U, Ts...>> {
    static constexpr int remainder = TypeIndex<T, TypeList<Ts...>>::remainder;
    static constexpr int value = (remainder==-1 ? -1 : TypeIndex<T, TypeList<Ts...>>::value+1);
    static_assert(value!=-1, "Can not find type in TypeList!");
};

template <class T, class... Ts>
struct TypeIndex<T, TypeList<T, Ts...>> {
    static constexpr int remainder = sizeof...(Ts);
    static constexpr int value = 0;
};

struct Color {};
struct Posistion {};
struct Alpha {};
struct Weight {};


using List = TypeList<Color, Posistion, Alpha, Weight>;

int main() {
    std::cout << "Color index in List: "     <<   TypeIndex<Color, List>::value     << "\n";
    std::cout << "Posistion index in List: " <<   TypeIndex<Posistion, List>::value << "\n";
    std::cout << "Alpha index in List: "     <<   TypeIndex<Alpha, List>::value     << "\n";
    std::cout << "Weight index in List: "    <<   TypeIndex<Weight, List>::value    << "\n";
}
