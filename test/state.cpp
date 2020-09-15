#include <cpp_template_gears/state.hpp>
#include <iostream>

struct BindVBO {
    using Type = size_t;
    static void Set(Type value) {
        std::cout << "set value: " << value << "\n";
    }

    static constexpr Type Default = 0;
};

int main(int argc, char **argv) {
    State<BindVBO> bindVBO;

    bindVBO = 1;
    bindVBO = 1;
    bindVBO = 2;
    bindVBO = 1;
    return 0;
} 
