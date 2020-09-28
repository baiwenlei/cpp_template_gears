#include <cpp_template_gears/actor/thread_local.hpp>
#include <thread>
#include <iostream>
#include <chrono>
#include <sstream>

using namespace actor;

std::string toString(size_t i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

class Client {
public:
    Client(std::string name, size_t age) : _name(std::move(name)) {
        _age = age;
        // std::cout << "full version constructor\n";
    }

    Client(size_t age) : Client("nobody", age) {
        // std::cout << "spical version constructor\n";
    }

public:
    std::string _name;
    size_t _age;
};

int main(int argc, const char** argv) {
	Client client{"main nobody", 10};
    ThreadLocal<Client*>::set(&client);

    auto lambda = [](std::string threadName, size_t age) {
        Client client{threadName, age};
        ThreadLocal<Client*>::set(&client);

        for (auto i=0u; i<10; ++i) {
            ThreadLocal<Client*>::get()->_name = threadName + toString(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
            std::cout << "thlClient set name: " << ThreadLocal<Client*>::get()->_name << "\n";
        }
    };

    std::thread a{lambda, "a", 20}, b{lambda, "b", 30};

    std::cout << "thlClient set name: " << ThreadLocal<Client*>::get()->_name << "\n";

    a.join();
    b.join();
	
	return 0;
}