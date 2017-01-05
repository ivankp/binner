#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <utility>
#include <type_traits>

template <typename T> struct is_std_array: std::false_type { };
template <typename T, size_t N>
struct is_std_array<std::array<T,N>>: std::true_type { };

template <typename T> struct is_std_vector: std::false_type { };
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T,Alloc>>: std::true_type { };

template <typename T, typename C>
struct foo {
  T m;
  C container;
  using value_type = typename C::value_type;

  template <typename... Args, typename C1 = C,
            typename = std::enable_if_t<is_std_array<C1>::value>>
  foo(Args&&... args)
  : m(std::forward<Args>(args)...), container{} {}

  template <typename... Args, typename C1 = C,
            typename = std::enable_if_t<is_std_vector<C1>::value>>
  foo(Args&&... args)
  : m(std::forward<Args>(args)...), container(std::get<0>(m)) {}
};

int main(int argc, char* argv[])
{
  foo<std::pair<int,std::string>,std::vector<double>> vec_foo;
  foo<std::pair<int,std::string>,std::array<double,5>> arr_foo;

  return 0;
}
