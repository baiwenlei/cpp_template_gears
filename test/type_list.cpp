#include <cpp_template_gears/type_list.hpp>
#include <iostream>

struct Color {};
struct Posistion {};
struct Alpha {};
struct Weight {};

using List = TypeList<Color, Posistion, Alpha, Weight>;

int main() {
  std::cout << "Color index in List: " << TypeIndex<Color, List>::value << "\n";
  std::cout << "Posistion index in List: " << TypeIndex<Posistion, List>::value
            << "\n";
  std::cout << "Alpha index in List: " << TypeIndex<Alpha, List>::value << "\n";
  std::cout << "Weight index in List: " << TypeIndex<Weight, List>::value
            << "\n";
}
