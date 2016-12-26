#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <cmath>
#include <cstdlib>

#include "axis.hh"

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;
#define test_cmp(var,val) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << ' ' \
            << ((var)==val ? "\033[32m✔" : "\033[31m✘" ) << "\033[0m" \
            << std::endl;
#define section(name) \
  std::cout << "\n\033[35m" name "\033[0m" << std::endl;
#define BR std::cout << std::endl;

using std::cout;
using std::endl;

union du {
  double d;
  uint64_t u;
};

std::string hex(double d) noexcept {
  static_assert(sizeof(uint64_t)==sizeof(double));
  du x;
  x.d = d;
  std::stringstream ss;
  ss << "0x" << std::hex << x.u;
  return ss.str();
}

template <unsigned I>
inline double prev(double x) noexcept {
  const double xn = prev<I-1>(x);
  return std::nextafter(xn,xn-1); 
}
template <>
inline double prev<1>(double x) noexcept {
  return std::nextafter(x,x-1); 
}
template <unsigned I>
inline double next(double x) noexcept {
  const double xn = next<I-1>(x);
  return std::nextafter(xn,xn+1); 
}
template <>
inline double next<1>(double x) noexcept {
  return std::nextafter(x,x+1); 
}

int main(int argc, char* argv[])
{
  cout << std::boolalpha;

  section("container_axis")

  ivanp::container_axis<std::vector<int>> a({1,2,3,4});
  ivanp::container_axis<std::vector<int>> a2 = a;
  // ivanp::container_axis<std::array<double,4>> a({1.,2.,3.,4.});

  test_cmp( sizeof(a2), (sizeof(std::vector<int>)+sizeof(void*)) )
  test_cmp( a2[2.5], 2 )
  test_cmp( a2.min(), 1 )
  test_cmp( a2.max(), 4 )

  section("ref_axis")

  ivanp::ref_axis<int> a3(&a);

  test_cmp( sizeof(a3), (2*sizeof(void*)) )
  test_cmp( a3[3.7], 3 )
  test_cmp( a3.has_underflow(), true )
  test_cmp( a3.has_overflow(), true )

  section("uniform_axis")

  ivanp::uniform_axis<double> a4(101,M_1_PI,M_PI);
  test(a4.min())
  test_cmp(hex(a4.min()),hex(M_1_PI))
  test(a4.max())
  test_cmp(hex(a4.max()),hex(M_PI))
  test_cmp(hex(a4.edge(0)),hex(M_1_PI))

  BR
  // const unsigned ie = 57;
  const unsigned ie = atoi(argv[1]);
  const auto edge = a4.edge(ie);
  // const auto edge_next = std::nextafter(edge,edge+1);
  // const auto edge_prev = std::nextafter(edge,edge-1);
  const auto edge_next = next<1>(edge);
  const auto edge_prev = prev<1>(edge);
  const auto prec_pow = std::pow(10,
    cout.precision() - unsigned(std::log10(std::abs(edge))) - 1);
  auto edge_trunc = std::trunc(edge*prec_pow);
  auto edge_trunc_next = (edge_trunc+1)/prec_pow;
  edge_trunc /= prec_pow;

  test(edge)
  test(edge_trunc)
  test(edge_trunc_next)
  test_cmp(a4[edge_trunc],ie-1)
  test_cmp(a4[edge_trunc_next],ie)

  test(hex(edge_prev))
  test(hex(edge))
  test(hex(edge_next))
  test_cmp(a4[edge_prev],ie-1)
  test_cmp(a4[edge],ie)
  test_cmp(a4[edge_next],ie)

  return 0;
}
