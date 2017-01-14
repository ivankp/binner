// Written by Ivan Pogrebnyak

#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <experimental/array>
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

  ivanp::binner<double> h1( {10,0,1} );
  print_type<decltype(h1)::axis_type<0>>();

  test( decltype(h1)::naxes )
  test( sizeof(h1) )
  test( h1.axis().nbins() )
  test( h1.axis().lower(2) )

  test_cmp( h1.find_bin(0.45), 5 )

  h1.fill_bin(1);
  h1.fill_bin({1});
  test_cmp( h1.bin(1), 2 )

  BR

  ivanp::binner<double, std::tuple<
    ivanp::axis_spec<ivanp::ref_axis<double>>,
    // ivanp::axis_spec<ivanp::container_axis<std::vector<double>>,false,false>
    ivanp::axis_spec<ivanp::container_axis<std::array<double,3>>,false,false>
  >/*, std::array<double,24>*/> h2( &h1.axis(), {1.,2.5,5.} );
  using h2_t = decltype(h2);

  // print_type<h2_t>(); BR
  print_type<h2_t::axis_type<0>>();
  print_type<h2_t::axis_type<1>>();

  test_cmp( h2_t::naxes, 2 )
  test_cmp( sizeof(h2),
    sizeof(h2_t::axis_type<0>) +
    sizeof(h2_t::axis_type<1>) +
    sizeof(h2_t::container_type) )
  test( sizeof(h2.axis<0>()) )
  test( sizeof(h2.axis<1>()) )
  test( sizeof(h2_t::container_type) )
  test( h2.nbins_total() )
  BR

  test_cmp( h2_t::axis_spec<0>::under::value, true );
  test_cmp( h2_t::axis_spec<0>::over::value, true );
  test_cmp( h2_t::axis_spec<1>::under::value, false );
  test_cmp( h2_t::axis_spec<1>::over::value, false );
  BR

  test_cmp( h2.bins().size(), 24 )
  test_cmp( h2.find_bin(0.45,2), 5+10*0 )
  test_cmp( h2.find_bin(0.45,3), 5+10*1 )

  h2.fill(0.35,3);
  test_cmp( h2.bins()[13], 0)
  test_cmp( h2.bins()[14], 1)
  test_cmp( h2.bin(4,1), 1 )

  h2.fill_bin(13);
  h2.fill_bin({3,1},4);
  test_cmp( h2.bins()[13], 5 )

  h2.fill(0.2,1.5,42);
  test_cmp( h2.bin(3,0), 42 )

  BR
  ivanp::binner<double, std::tuple<
    ivanp::axis_spec<const ivanp::uniform_axis<double>&>
  >> h3( h1.axis() );
  print_type<decltype(h3)::axis_spec<0>>();

  test( decltype(h3)::naxes )
  test( sizeof(h3) )
  test( h3.axis().nbins() )
  test( h3.axis().min() )
  test( h3.axis().max() )

  return 0;
}
