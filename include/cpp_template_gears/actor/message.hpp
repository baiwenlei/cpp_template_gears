#pragma once

#include <future>
#include <tuple>

namespace actor {

class Message {
public:
    virtual ~Message() = default;
    virtual void operator()() = 0;
};

template <class Fn, class ApplyTuple>
class MessageImpl : public Message {
public:
    MessageImpl(Fn memFn, ApplyTuple applyArgs) : _applyArgs(std::move(applyArgs)) {
        _memFn = memFn;
    }

    void operator()() override {
        std::apply(_memFn, std::move(_applyArgs));
    }
private:
    Fn _memFn;
    ApplyTuple _applyArgs;
};

template <class R, class Fn, class ApplyTuple>
class AskMessageImpl : public Message {
public:
    AskMessageImpl(std::promise<R> promise, Fn memFn, ApplyTuple applyArgs) : _applyArgs(std::move(applyArgs)) {
        _promise = std::move(promise);
        _memFn = memFn;
    }

    void operator()() override {
        auto ret = std::apply(_memFn, std::move(_applyArgs));
        _promise.set_value(ret);
    }
private:
    std::promise<R> _promise;
    Fn _memFn;
    ApplyTuple _applyArgs;
};

template <class Fn, class ApplyTuple>
class AskMessageImpl<void, Fn, ApplyTuple> : public Message {
public:
    AskMessageImpl(std::promise<void> promise, Fn memFn, ApplyTuple applyArgs) : _applyArgs(std::move(applyArgs)) {
        _promise = std::move(promise);
        _memFn = memFn;
    }

    void operator()() override {
        std::apply(_memFn, std::move(_applyArgs));
        _promise.set_value();
    }

private:
    std::promise<void> _promise;
    Fn _memFn;
    ApplyTuple _applyArgs;
};

template <class Object, class Fn, class... Args>
std::unique_ptr<Message> make_message(Object &object, Fn memFn, Args&&... args) {
    auto applyTuple = std::make_tuple(std::ref(object), std::forward<Args>(args)...);
    return std::make_unique<MessageImpl<Fn, decltype(applyTuple)>>(memFn, std::move(applyTuple));
}

template <class Object, class Fn, class... Args>
std::unique_ptr<Message> make_message(Object &object, std::promise<FunctionRetType<Fn>> promise, Fn memFn, Args&&... args) {
    auto applyTuple = std::make_tuple(std::ref(object), std::forward<Args>(args)...);
    return std::make_unique<AskMessageImpl<FunctionRetType<Fn>, Fn, decltype(applyTuple)>>(std::move(promise), memFn, std::move(applyTuple));
}

}
