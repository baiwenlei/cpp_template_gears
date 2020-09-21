#include <cpp_template_gears/index_sequence.hpp>
#include <iostream>
#include <tuple>
#include <initializer_list>

using namespace cpp_template_gears;

template <class... Ts>
class Message {
public:
    Message(Ts... args) : _params(std::forward_as_tuple(args...)) {};

    void operator()() const {
        invoke(make_index_sequence<sizeof...(Ts)>{});
    }

private:
    template <size_t... I>
    void invoke(index_sequence<I...> unused) const {
        double result = 0;

        std::initializer_list<int>{
            (result+=std::get<I>(_params), 0)...
        };

        std::cout << result << "\n";
    }

private:
    std::tuple<Ts...> _params;
};


int main(int argc, char **argv) {
    Message<int, short, char> message{100, 2, 9};

    message();
} 
