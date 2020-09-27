#pragma once
#include <cpp_template_gears/actor/function.hpp>
#include <cpp_template_gears/actor/mailbox.hpp>
#include <cpp_template_gears/actor/message.hpp>
#include <future>
#include <memory>

namespace actor {

template <class Object> class ActorRef {
public:
  ActorRef(Object &obj, std::weak_ptr<Mailbox> mailbox) {
    _obj = &obj;
    _mailbox = std::move(mailbox);
  }

  template <class Fn, class... Args> void invoke(Fn memFn, Args &&... args) {
    auto strongRef = _mailbox.lock();
    if (strongRef) {
      strongRef->push(
          make_message(*_obj, memFn, std::forward<Args &&>(args)...));
    }
  }

  template <class Fn, class... Args>
  void invoke(std::promise<FunctionRetType<Fn>> promise, Fn memFn,
              Args &&... args) {
    auto strongRef = _mailbox.lock();
    if (strongRef) {
      strongRef->push(make_message(*_obj, std::move(promise), memFn,
                                   std::forward<Args &&>(args)...));
    }
  }

private:
  Object *_obj;
  std::weak_ptr<Mailbox> _mailbox;
};

} // namespace actor
