#include <iostream>
#include <vector>
#include <array>

template <typename C>
struct foo {
  C container;

  template <typename Arg>
  foo(Arg&& arg): container(1,std::forward<Arg>(arg)) {
    std::cout << "Vector constructor" << std::endl;
  }
};

// template <typename T, size_t N>
// template <typename Arg>
// foo<std::array<T,N>>::foo(Arg&& arg) {
//   for (auto& x : container) x = std::forward<Arg>(arg);
//   std::cout << "Array constructor" << std::endl;
// }

int main(int argc, char* argv[])
{
  foo<std::vector<double>>  v(5);
  // foo<std::array<double,1>> a(7);

  return 0;
}
