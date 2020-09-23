#pragma once
#include <array>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace cpp_template_gears {

template <size_t N>
class ThreadPool final {
public:
    ThreadPool() {
        for (auto i=0u; i<N; ++i) {
            _threads[i] = std::thread{[&, i]{
                runOnThread(i);
            }};
        }
    }

    ~ThreadPool() {
        quit();
        for (auto &thread : _threads) {
            thread.join();
        }
    }

    void quit() {
        {
            std::lock_guard<std::mutex> guard{_mutex};
            _quit = true;
        }
        _cond.notify_all();
    }

    void schedule(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> guard{_mutex};
            _tasks.push(std::move(task));
        }
        _cond.notify_all();
    }


private:
    void runOnThread(size_t number) {
        while (true) {
            std::unique_lock<std::mutex> lock{_mutex};
            
            std::cout << "thread " << number << " start running" << "\n";

            _cond.wait(lock, [&]{
                return _quit || !_tasks.empty();
            });

            if (_quit) {
                std::cout << "thread " << number << " quitting" << "\n";
                break;
            }

            auto task = std::move(_tasks.front());
            _tasks.pop();
            lock.unlock();
            if (task) {
                task();
            }
        }
    }

private:
    bool _quit = false;
    std::mutex _mutex;
    std::condition_variable _cond;
    std::queue<std::function<void()>> _tasks;
    std::array<std::thread, N> _threads;
};

}
