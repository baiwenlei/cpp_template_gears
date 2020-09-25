#include <iostream>
#include <thread>
#include <queue>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <array>
#include <memory>
#include <mutex>
#include <future>

class Mailbox;

class Scheduler {
public:
    Scheduler() = default;
    virtual ~Scheduler() = default;
    virtual void schedule(std::weak_ptr<Mailbox> mailbox) = 0;
};

class Message {
public:
    virtual ~Message() = default;
    virtual void operator()() = 0;
};

template <class Object, class Fn, class ArgsTuple>
class MessageImpl : public Message {
public:
    MessageImpl(Object& obj, Fn memFn, ArgsTuple args) : _obj(obj) {
        _memFn = memFn;
        _args = std::move(args);
    }

    void operator()() override {
        invoke(std::make_index_sequence<std::tuple_size<ArgsTuple>::value>{});
    }

private:
    template <size_t... I>
    void invoke(std::index_sequence<I...> unused) {
        (_obj.*_memFn)(std::move(std::get<I>(_args))...);
    }

private:
    Object &_obj;
    Fn _memFn;
    ArgsTuple _args;
};

template <class Object, class T, class Fn, class ArgsTuple>
class AskMessageImpl;

template <class Object, class Fn, class ArgsTuple>
class AskMessageImpl<Object, void, Fn, ArgsTuple> : public Message {
public:
    AskMessageImpl(Object &object, Fn memFn, std::promise<void> promise, ArgsTuple args) : _object(object) {
        _memFn = memFn;
        _promise = std::move(promise);
        _args = std::move(args);
    }

    void operator()() override {
        invoke(std::make_index_sequence<std::tuple_size<ArgsTuple>::value>{});
    }

private:
    template <size_t... I>
    void invoke(std::index_sequence<I...> unused) {
        (_object.*_memFn)(std::move(std::get<I>(_args))...);
        _promise.set_value();
    }

private:
    Object &_object;
    Fn _memFn;
    ArgsTuple _args;
    std::promise<void> _promise;
};

template <class Fn>
struct FunctionType;

template <class Object, class R, class... Args>
struct FunctionType<R(Object::*)(Args...)> {
    using RetType = R;
};

template <class Fn>
using FunctionRetType = typename FunctionType<Fn>::RetType;

template <class Object, class Fn, class... Args>
auto make_message(Object &obj, Fn memFn, Args&&... args) {
    auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
    return std::make_unique<MessageImpl<Object, Fn, decltype(argsTuple)>>(obj, memFn, std::move(argsTuple));
}

template <class Object, class Fn, class... Args>
auto make_message(Object &obj, std::promise<FunctionRetType<Fn>> promise, Fn memFn, Args&&... args) {
    auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
    return std::make_unique<AskMessageImpl<Object, FunctionRetType<Fn>, Fn, decltype(argsTuple)>>(obj, memFn, std::move(promise), std::move(argsTuple));
}

class Mailbox final : public std::enable_shared_from_this<Mailbox> {
public:
    Mailbox(Scheduler &scheduler) : _scheduler(scheduler) {}

    void push(std::unique_ptr<Message> message) {
        std::unique_lock<std::recursive_mutex> lock{_mutex};
        auto wasEmpty = _queue.empty();
        _queue.push(std::move(message));
        lock.unlock();

        if (wasEmpty) {
            _scheduler.schedule(this->shared_from_this());
        }
    }

    void receive() {
        std::unique_lock<std::recursive_mutex> lock{_mutex};
        if (_queue.empty()) {
            return;
        }

        auto message = std::move(_queue.front());
        _queue.pop();
        if (!_queue.empty()) {
            _scheduler.schedule(this->shared_from_this());
        } 
        lock.unlock();

        if (message) {
            (*message)();
        }
    }

private:
    std::recursive_mutex _mutex;
    std::queue<std::unique_ptr<Message>> _queue;
    Scheduler &_scheduler;
};

class ThreadSchedulerBase : public Scheduler {
public:
    ThreadSchedulerBase() = default;
    virtual ~ThreadSchedulerBase() = default;

    void schedule(std::weak_ptr<Mailbox> mailbox) override {
        {
            std::lock_guard<std::mutex> guard{_mutex};
            _queue.push([mailbox]{
                auto strong = mailbox.lock();
                if (strong) {
                    strong->receive();
                }
            });
        }
        _cond.notify_all();
    }

    void quit() {
        {
            std::lock_guard<std::mutex> guard{_mutex};
            _quit = true;
        }
        _cond.notify_all();
    }

    std::thread makeThread(size_t number) {
        auto lambda = [&, number] {
                        std::cout << "thread " << number << " id: " << std::this_thread::get_id() << "\n";
                        while (true) {
                            std::cout << "thread " << number << " is running\n";
                            std::unique_lock<std::mutex> lock(_mutex);
                            _cond.wait(lock, [&] {
                                return _quit || !_queue.empty();
                            });

                            if (_quit) {
                                std::cout << "thread " << number << " quiting\n";
                                break;
                            }

                            auto task = std::move(_queue.front());
                            _queue.pop();
                            lock.unlock();
                            if (task) {
                                task();
                            }
                        };
                    };

        return std::thread{lambda};
    }

private:
    bool _quit = false;
    std::mutex _mutex;
    std::condition_variable _cond;
    std::queue<std::function<void()>> _queue;
};

template <size_t N>
class ThreadScheduler : public ThreadSchedulerBase {
public:
    ThreadScheduler() {
        for(auto i=0u; i<N; ++i) {
            _threads[i] = makeThread(i);
        }
    }

    ~ThreadScheduler() {
        quit();
        for(auto &thread : _threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

private:
    std::array<std::thread, N> _threads;
};

using SequenceScheduler = ThreadScheduler<1>;
using ThreadPool = ThreadScheduler<3>;

using Data = std::vector<int>;
using Layers = std::vector<std::string>;

template <class Object>
class ActorRef {
public:
    ActorRef(Object &object, std::weak_ptr<Mailbox> mailbox) {
        _object = &object;
        _mailbox = std::move(mailbox);
    }

    template <class Fn, class... Args>
    void invoke(Fn fn, Args&&... args) {
        auto strongRef = _mailbox.lock();
        if (strongRef) {
            strongRef->push(make_message(*_object, fn, std::forward<Args>(args)...));
        }
    }

    template <class Fn, class... Args> 
    void invoke(std::promise<FunctionRetType<Fn>> promise, Fn fn, Args&&... args) {
        auto strongRef = _mailbox.lock();
        if (strongRef) {
            strongRef->push(make_message(*_object, std::move(promise), fn, std::forward<Args>(args)...));
        }
    }

private:
    Object *_object;
    std::weak_ptr<Mailbox> _mailbox;
};



class Client {
public:
    Client() = default;

    void setData(std::unique_ptr<Data> data) {
        std::cout << "set data get called at thread: " << std::this_thread::get_id() << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        _data = std::move(data);
        std::cout << "data size: " << _data->size() << "\n";
    }

    void setLayers(std::unique_ptr<Layers> layers) {
        std::cout << "set layers get called at thread: " << std::this_thread::get_id() << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        _layers = std::move(layers);
        std::cout << "layers size: " << _layers->size() << "\n";
    }

private:
    std::unique_ptr<Data> _data;
    std::unique_ptr<Layers> _layers;
};

int main(int argc, char **argv) {
    ThreadPool threadpool;
    auto mailbox = std::make_shared<Mailbox>(threadpool);

    Client client;
    ActorRef<Client> actor{client, mailbox};


    for(auto i=0u; i<10; ++i) {
        // actor.invoke(&Client::setData, std::make_unique<Data>(std::initializer_list<int>{1, 2, 3, 4}));
        std::promise<void> promise;
        auto future = promise.get_future();
        actor.invoke(std::move(promise), &Client::setData, std::make_unique<Data>(std::initializer_list<int>{1, 2, 3, 4}));
        future.wait();
        std::cout << "promise returned from " << i << "\n";
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
} 
