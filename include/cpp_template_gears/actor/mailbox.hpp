#pragma once
#include <cpp_template_gears/actor/message.hpp>
#include <cpp_template_gears/actor/scheduler.hpp>
#include <memory>
#include <mutex>
#include <queue>

namespace actor {

class Mailbox : public std::enable_shared_from_this<Mailbox> {
public:
  Mailbox(Scheduler &scheuler) : _scheduler(scheuler) {}

  void push(std::unique_ptr<Message> message) {
    std::unique_lock lock(_mutex);
    auto wasEmpty = _queue.empty();
    _queue.push(std::move(message));
    lock.unlock();

    if (wasEmpty) {
      _scheduler.scheduleMailbox(shared_from_this());
    }
  }

  void receive() {
    std::unique_lock lock(_mutex);
    if (_queue.empty()) {
      return;
    }

    auto message = std::move(_queue.front());
    _queue.pop();
    auto empty = _queue.empty();
    lock.unlock();
    if (!empty) {
      _scheduler.scheduleMailbox(shared_from_this());
    }

    if (message) {
      (*message)();
    }
  }

private:
  Scheduler &_scheduler;
  std::recursive_mutex _mutex;
  std::queue<std::unique_ptr<Message>> _queue;
};

} // namespace actor
