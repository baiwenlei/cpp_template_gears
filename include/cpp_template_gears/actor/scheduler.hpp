#pragma once

#include <memory>

namespace actor {

class Mailbox;

class Scheduler {
public:
  virtual ~Scheduler() = default;
  virtual void scheduleMailbox(std::weak_ptr<Mailbox> mailbox) = 0;
};

} // namespace actor
