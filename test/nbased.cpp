#include <cpp_template_gears/binary.hpp>
#include <cpp_template_gears/nbased.hpp>
#include <iostream>

int main() {
  constexpr auto bvalue = BinaryCreator<5>::Type::value();
  std::cout << bvalue << "\n";

  constexpr auto nvalue = NBaseSeqCreator<16, 8>::Type::value();
  std::cout << nvalue << "\n";
}
