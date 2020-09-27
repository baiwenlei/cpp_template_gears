#pragma once

namespace actor {

///function.hpp
template <class T>
struct FunctionTrait;

template <class R, class... Args>
struct FunctionTrait<R(*)(Args...)> {
    using ret_type = R;
};

template <class Object, class R, class... Args>
struct FunctionTrait<R(Object::*)(Args...)> {
    using ret_type = R;
};

template <class Object, class R, class... Args>
struct FunctionTrait<R(Object::*)(Args...)const> {
    using ret_type = R;
};

template <class T>
using FunctionRetType = typename FunctionTrait<T>::ret_type;

}
