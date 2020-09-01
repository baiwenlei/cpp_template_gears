#include <iostream>
#include <tuple>

constexpr size_t StringLength(const char *str, size_t len=0) {
    return (str[len]=='\0') ? len : StringLength(str, len+1);
}

template <char... chars>
struct LiteralString {
    static constexpr const char buf[] = {chars..., 0};

    static constexpr const char* value() {
        return buf;
    }
};

namespace detail {
    using StringGetFunction = const char* (*)();

    template <StringGetFunction fn, size_t... I>
    constexpr LiteralString<fn()[I]...> MakeStringLiteral(std::index_sequence<I...>);

    template <class... Ts>
    struct StringConcat;

    template <char... chars1>
    struct StringConcat<LiteralString<chars1...>> {
        using type = LiteralString<chars1...>;
    };

    template <char... chars1, char... chars2, class... Ts>
    struct StringConcat<LiteralString<chars1...>, LiteralString<chars2...>, Ts...> {
        using type = typename StringConcat<LiteralString<chars1..., chars2...>, Ts...>::type;
    };
}

template <detail::StringGetFunction... fns>
struct StringConcat {
    using type = typename detail::StringConcat< 
                                    decltype(detail::MakeStringLiteral<fns>(std::make_index_sequence<StringLength(fns())>{}))...
                                    >::type;
    
    static constexpr const char* value() {
        return type::value();
    }
};

constexpr const char* hello() {
    return "hello";
}
constexpr const char* world() {
    return "world";
}

int main() {
    std::cout << StringConcat<&hello, &LiteralString<' ', 'x', ' '>::value, &world>::value() << "\n";
}
