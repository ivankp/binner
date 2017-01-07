#include <iostream>
#include <vector>
#include <array>
#include <utility>
#include <type_traits>

template <typename T> struct is_std_array: std::false_type { };
template <typename T, size_t N>
struct is_std_array<std::array<T,N>>: std::true_type { };

template <typename C>
struct foo {
  C container;

  foo(const C& x): container(x)
  { std::cout << "const C&" << std::endl; }

  template <typename T, typename C1 = C,
            typename = std::enable_if_t<!is_std_array<C1>::value>>
  foo(std::initializer_list<T> x): container(x)
  { std::cout << "std::initializer_list<T>" << std::endl; }
};

int main() {
  foo<std::vector<int>> vec_foo({1,2,3,4,5});
  foo<std::array<int,5>> arr_foo({1,2,3,4,5});
}

