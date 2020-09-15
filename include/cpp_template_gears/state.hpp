#pragma once

#include <tuple>

template <class T, class... Ts>
class State {
public:
    State(Ts... ts) : _params(ts...) {}

    void operator=(typename T::Type value) {
        if (*this != value) {
            setCurrentVlaue(value);
        }
    }

    bool operator!= (typename T::Type value) const {
        return _dirty || (_currentValue != value);
    }


    void setCurrentVlaue(typename T::Type value) {
        _dirty = false;
        set(value, std::make_index_sequence<sizeof...(Ts)>{});
    }

private:
    template <size_t... I>
    void set(typename T::Type value, std::index_sequence<I...> unused) {
        _currentValue = value;
        T::Set(value, std::get<I>(_params)...);
    }

private:
    typename T::Type _currentValue = T::Default;
    std::tuple<Ts...> _params;
    bool _dirty = true;
};
