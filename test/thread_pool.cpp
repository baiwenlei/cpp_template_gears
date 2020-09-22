#include <cpp_template_gears/thread_pool.hpp>
#include <iostream>

using namespace cpp_template_gears;

int main(int argc, char **argv) {
    ThreadPool<3> pool;

    for (auto i=0u; i<10; ++i) {
        pool.schedule([]{
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "get running on thread " << std::this_thread::get_id() << "\n";
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3));

    pool.quit();

    return 0;
} 
