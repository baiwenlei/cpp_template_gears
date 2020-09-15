#include <cpp_template_gears/string_literal.hpp>
#include <iostream>

constexpr const char *hello() { return "hello"; }
constexpr const char *world() { return "world"; }

int main() {
  std::cout << StringConcat<&hello, &LiteralString<' ', 'x', ' '>::value,
                            &world>::value()
            << "\n";
  return 0;
}