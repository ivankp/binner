// Written by Ivan Pogrebnyak

#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
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
#define section(name) \
  std::cout << "\n\033[1;35m" << name << "\033[0m" << std::endl;
#define BR std::cout << std::endl;

using std::cout;
using std::endl;

template <typename T>
void print_type() {
  std::cout << "\033[1;35m"
    << boost::typeindex::type_id<T>().pretty_name()
    << "\033[0m" << std::endl;
}

#include "axis.hh"

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

  // ================================================================
  BR
  ivanp::container_axis<std::vector<int>> a1({1,2,3,4});
  print_type<decltype(a1)>();

  // ivanp::container_axis<std::vector<int>> a2 = a1;
  // ivanp::container_axis<std::array<double,4>> a1({1.,2.,3.,4.});

  test_cmp( sizeof(a1), (sizeof(std::vector<int>)+sizeof(void*)) )
  test_cmp( a1[0.5], 0 )
  test_cmp( a1[2.5], 2 )
  test_cmp( a1[4.5], 4 )
  test_cmp( a1.min(), 1 )
  test_cmp( a1.max(), 4 )

  BR
  ivanp::container_axis<const std::vector<int>&> a2(a1.edges());
  print_type<decltype(a2)>();

  // ivanp::container_axis<std::vector<int>> a2 = a1;
  // ivanp::container_axis<std::array<double,4>> a1({1.,2.,3.,4.});

  test_cmp( sizeof(a2), (sizeof(void*)*2) )
  test_cmp( a2[0.5], 0 )
  test_cmp( a2[2.5], 2 )
  test_cmp( a2[4.5], 4 )
  test_cmp( a2.min(), 1 )
  test_cmp( a2.max(), 4 )

  // ================================================================
  BR
  ivanp::ref_axis<int> a3(&a1);
  print_type<decltype(a3)>();

  test_cmp( sizeof(a3), (2*sizeof(void*)) )
  test_cmp( a3[3.7], 3 )

  { // ==============================================================
  BR
  const unsigned nbins = 101;
  auto a = ivanp::make_axis(nbins,-M_1_PI,537*M_PI);
  print_type<decltype(a)>();

  test( a[3.7] )
  test(a.min())
  // test_cmp(hex(a4.min()),hex(M_1_PI))
  test(a.max())
  // test_cmp(hex(a4.max()),hex(M_PI))
  // test_cmp(hex(a4.edge(0)),hex(M_1_PI))
  test_cmp(a[a.min()],1)
  test_cmp(a[a.min()-100.],0)
  test_cmp(a[a.max()],nbins+1)
  test_cmp(a[a.max()+100.],nbins+1)

  BR
  std::unordered_map<int,int> epsilons;
  for (unsigned i=0; i<nbins+1; ++i) {
    auto x = a./*exact_*/edge(i);
    const auto j = a[x]-1;

    int n = (j < i ? -1 : 0);
    if (j < i) for ( ; a[x = next<1>(x)] <  i+1; ) --n;
    else       for ( ; a[x = prev<1>(x)] == i+1; ) ++n;
  
    /*if (n) {
      cout << "Edge " << i;
      if (j < i) cout << " behind";
      else       cout << " ahead";
      cout << " by " << n << endl;
    }*/
    ++epsilons[n];
  }
  cout << "ϵ deviations from true edges" << endl;
  for (auto& e : epsilons) cout << e.first << ": " << e.second << endl;
  BR

  // const unsigned ie = 1;
  // const auto edge = a.edge(ie);
  // const auto edge_next = next<1>(edge);
  // const auto edge_prev = prev<1>(edge);
  // const auto prec_pow = std::pow(10,
  //   cout.precision() - int(std::log10(std::abs(edge))) - 1);
  // auto edge_trunc = std::trunc(edge*prec_pow);
  // auto edge_trunc_next = (edge_trunc+1)/prec_pow;
  // edge_trunc /= prec_pow;
  //
  // test(edge)
  // test(edge_trunc)
  // test(edge_trunc_next)
  // test_cmp(a[edge_trunc],ie)
  // test_cmp(a[edge_trunc_next],ie+1)
  //
  // test(hex(edge_prev))
  // test(hex(edge))
  // test(hex(edge_next))
  // test_cmp(a[edge_prev],ie)
  // test_cmp(a[edge],ie+1)
  // test_cmp(a[edge_next],ie+1)
  }

  { // constexpr_axis ===============================================
    static_assert(ivanp::const_axis<double>{{0.,0.25,0.5,0.75,1.}}.nbins() == 4);
    constexpr ivanp::const_axis<double> a((const double[]){0.,0.25,0.5,0.75,1.});
    test_cmp( a.nbins(), 4 )
    static_assert(a.nbins()==4);
    test_cmp( a[-1], 0 )
    static_assert(a[-1]==0);
    test_cmp( a[2], 5 )
    // static_assert(a[1.5]==5);
    test_cmp( a[0.3], 2 )
    // static_assert(a[0.3]==2);
    test_cmp( a[0.5], 3 )
  }

  return 0;
}
