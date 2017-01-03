// Written by Ivan Pogrebnyak

#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <cmath>
#include <cstdlib>

#include <boost/type_index.hpp>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;
#define test_cmp(var,val) { \
  const auto x = var; \
  std::cout << "\033[36m" << #var << "\033[0m = " << x << ' ' \
            << (x==val ? "\033[32m✔" : "\033[31m✘" ) << "\033[0m" \
            << std::endl; }
#define BR std::cout << std::endl;

using std::cout;
using std::endl;

template <typename T>
void print_type() {
  std::cout << "\033[1;35m"
    << boost::typeindex::type_id<T>().pretty_name()
    << "\033[0m" << std::endl;
}

#include "binner.hh"

int main(int argc, char* argv[])
{
  cout << std::boolalpha;

  // test wrong template argument
  // ivanp::binner<double, ivanp::uniform_axis<int>> h;

  {
  ivanp::binner<double> h( std::make_tuple(10,0,1) );
  print_type<decltype(h)::axis_type<0>>();

  test( decltype(h)::naxes )
  test( sizeof(h) )
  test( h.axis().nbins() )
  test( h.axis().lower(2) )
  }

  {
  BR

  ivanp::binner<double, std::tuple<
    ivanp::axis_spec<ivanp::uniform_axis<double>>,
    ivanp::axis_spec<ivanp::container_axis<std::vector<double>>,false,false>
  >> h( std::make_tuple(10,0,1),
        std::make_tuple(std::initializer_list<double>{1.,2.5,5.}) );
  // print_type<decltype(h)>(); BR
  print_type<decltype(h)::axis_type<0>>();
  print_type<decltype(h)::axis_type<1>>();

  test( decltype(h)::naxes )
  test( sizeof(h) )
  test( decltype(h)::axis_spec<0>::under::value );
  test( decltype(h)::axis_spec<0>::over::value );
  test( decltype(h)::axis_spec<1>::under::value );
  test( decltype(h)::axis_spec<1>::over::value );
  }

  return 0;
}
