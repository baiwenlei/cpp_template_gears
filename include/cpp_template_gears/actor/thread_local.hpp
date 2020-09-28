#pragma once

#include <utility>

namespace actor {

template <class T>
class ThreadLocal {
public:
    static T& get() {
        return object;
    }

    static void set(T object_) {
        object = std::move(object_);
    }

private:
    static thread_local T object;
};

template <class T>
thread_local T ThreadLocal<T>::object;

}