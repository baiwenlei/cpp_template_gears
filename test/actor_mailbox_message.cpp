#include <chrono>
#include <cpp_template_gears/actor/actor_ref.hpp>
#include <cpp_template_gears/actor/mailbox.hpp>
#include <cpp_template_gears/actor/thread_scheduler.hpp>
#include <vector>

using namespace actor;

class Client {
public:
  Client() = default;

  void setData(std::unique_ptr<std::vector<int>> data) {
    std::cout << "set data get called at thread: " << std::this_thread::get_id()
              << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    _data = std::move(data);
    std::cout << "data size: " << _data->size() << "\n";
  }

private:
  std::unique_ptr<std::vector<int>> _data;
};

int main(int argc, char **argv) {
  Client client;
  ThreadPool threadpool;
  auto mailbox = std::make_shared<Mailbox>(threadpool);

  ActorRef<Client> actor{client, mailbox};

  for (auto i = 0u; i < 1000u; ++i) {
    std::promise<void> promise;
    auto future = promise.get_future();
    actor.invoke(std::move(promise), &Client::setData,
                 std::make_unique<std::vector<int>>(
                     std::initializer_list<int>{1, 2, 3, 4}));
    future.wait();
    std::cout << "promise returned from " << i << "\n";
  }

  //   std::this_thread::sleep_for(std::chrono::seconds(1));

  return 0;
}
