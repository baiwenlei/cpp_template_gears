#include <cpp_template_gears/gcd.hpp>
#include <iostream>

int main() {
  constexpr auto value = GCD<2, 8>::value;
  std::cout << value << "\n";
}