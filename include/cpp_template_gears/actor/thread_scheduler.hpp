#pragma once
#include <array>
#include <condition_variable>
#include <cpp_template_gears/actor/mailbox.hpp>
#include <cpp_template_gears/actor/scheduler.hpp>
#include <functional>
#include <iostream>
#include <mutex>

namespace actor {

class ThreadSchedulerBase : public Scheduler {
public:
  std::thread makeThread() {
    return std::thread{[&]() {
      std::cout << "running thread " << std::this_thread::get_id() << "\n";
      while (true) {
        std::unique_lock lock(_mutex);

        _cond.wait(lock, [&] { return _quit || !_queue.empty(); });

        if (_quit) {
          std::cout << "quiting thread " << std::this_thread::get_id() << "\n";
          return;
        }

        auto task = std::move(_queue.front());
        _queue.pop();
        lock.unlock();

        if (task) {
          task();
        }
      }
    }};
  }

  virtual void quit() {
    {
      std::lock_guard<std::mutex> guard(_mutex);
      _quit = true;
    }

    _cond.notify_all();
  }

  void push(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> guard(_mutex);
      _queue.push(std::move(task));
    }
    _cond.notify_all();
  }

private:
  bool _quit = false;
  std::mutex _mutex;
  std::condition_variable _cond;
  std::queue<std::function<void()>> _queue;
};

template <size_t N> class ThreadScheduler : public ThreadSchedulerBase {
public:
  ThreadScheduler() {
    for (auto &thread : _threads) {
      thread = makeThread();
    }
  }

  ~ThreadScheduler() {
    ThreadSchedulerBase::quit();
    join();
  }

  void join() {
    for (auto &thread : _threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  void scheduleMailbox(std::weak_ptr<Mailbox> mailbox) override {
    push([mailbox] {
      auto strongRef = mailbox.lock();
      if (strongRef) {
        strongRef->receive();
      }
    });
  }

private:
  std::array<std::thread, N> _threads;
};

using ThreadPool = ThreadScheduler<3>;

} // namespace actor
