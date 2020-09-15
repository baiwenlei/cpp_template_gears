#include <cpp_template_gears/indexed_tuple.hpp>
#include <iostream>

struct color{};
struct position{};
struct alpha{};

struct color2{};
struct position2{};
struct alpha2{};


using PaintProperties = IndexedTuple<TypeList<color, position, alpha>, TypeList<int, float, std::string>>;
using PaintProperties2 = IndexedTuple<TypeList<alpha2, position2, color2>, TypeList<int, float, std::string>>;


int main(int argc, char **argv) {
    PaintProperties properties;
    std::cout << properties.get<color>() << "\n";
    std::cout << properties.get<position>() << "\n";
    std::cout << properties.get<alpha>() << "\n";

    properties.get<alpha>() = "hello index tuple";
    std::cout << properties.get<alpha>() << "\n";

    PaintProperties2 properties2{200, 200.0f, "tuple2"};

    auto concat_properties = properties.concat(properties2);
    std::cout << concat_properties.get<color>() << "\n";
    std::cout << concat_properties.get<position>() << "\n";
    std::cout << concat_properties.get<alpha>() << "\n";
    std::cout << concat_properties.get<color2>() << "\n";
    std::cout << concat_properties.get<position2>() << "\n";
    std::cout << concat_properties.get<alpha2>() << "\n";


    return 0;
} 
