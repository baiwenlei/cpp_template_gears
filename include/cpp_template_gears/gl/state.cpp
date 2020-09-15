#include <iostream>
#include <type_traits>
#include <tuple>
#include <vector>

template <class T>
struct unimplement;

struct Context {
    int version;

    Context(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(const Context&) = delete;
    Context& operator=(Context&&) = delete;
};

using AttributeLocation = int;

struct VertexAttribute {
    using type = int;
    static void Set(const type& val, Context &context, AttributeLocation location) {
        std::cout << "val: " << val 
                  << " context " << context.version
                  << " location: " << location
                  << "\n";
    }

    static const type Default = 100;
};

template <class T, class... Args>
class State {
public:
    using ValueType = typename T::type;

    State(Args... args) : _args{std::forward<Args>(args)...} 
    {
    }

    void Set(const ValueType &value) {
        if (_current_value != value) {
            _current_value = value;
            Set(std::make_index_sequence<sizeof...(Args)>{});
        }
    }

private:
    template <size_t... I>
    void Set(std::index_sequence<I...> unused) {
        T::Set(_current_value, std::get<I>(_args)...);
    }

private:
    typename T::type    _current_value = T::Default;
    std::tuple<Args...> _args;
};


int main() {
    Context context{101};
    AttributeLocation loc0 = 0;

    using AttributeState = State<VertexAttribute, Context&, AttributeLocation>;

    AttributeState state{context, loc0};
    state.Set(100);

    state.Set(200);
    state.Set(200);
    state.Set(100);

    std::vector<AttributeState> states;
    states.push_back(state);

    std::vector<std::tuple<int&, float&>> tuples;
}
