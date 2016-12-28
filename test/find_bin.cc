#include <iostream>
#include <vector>
#include <array>
#include <experimental/array>

#include <boost/type_index.hpp>

#include "axis.hh"

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

template <typename T>
void print_type() {
  std::cout << "\033[1;35m"
    << boost::typeindex::type_id<T>().pretty_name()
    << "\033[0m" << std::endl;
}

int main(int argc, char* argv[])
{
  auto a = ivanp::make_axis(std::experimental::make_array(1,2,3,4));
  print_type<decltype(a)>();

  test(sizeof(a))
  
  a.find_bin(1.5);
  a[2.5];

  std::cout << std::endl;
  ivanp::container_axis<std::vector<int>> a1({1,2,3,4});
  print_type<decltype(a1)>();

  test( a1[0.5] )
  test( a1[2.5] )
  test( a1[4.5] )

  return 0;
}
