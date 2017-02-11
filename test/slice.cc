// Written by Ivan Pogrebnyak

#include <iostream>
#include <iomanip>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

#include "slice.hh"

using std::cout;
using std::endl;

int main()
{
  ivanp::binner<double, std::tuple<
    ivanp::axis_spec<ivanp::uniform_axis<double>,false,false>,
    ivanp::axis_spec<ivanp::uniform_axis<int>>,
    ivanp::axis_spec<ivanp::uniform_axis<float>,false,false>
  >> hist( {3,0,10}, {5,-2,3}, {2,0,5} );

  test( hist.nbins_total() )

  for (auto& bin : hist.bins()) {
    static int i = 0;
    bin = ++i;
  }

  // const auto h1 = ivanp::slice(hist);
  // const auto h1 = ivanp::slice<1>(hist,std::index_sequence<1,2,0>{});
  const auto h1 = ivanp::slice<1,1,2,0>(hist);

  test( sizeof(decltype(h1)::value_type) )

  for (const auto& h : h1) {
    cout << std::setprecision(3);
    cout << h.name("2",0) << endl;

    cout << "nbins = " << h.bins.size() << endl;

    cout << "edges";
    for (const auto e : h.get_edges<0>()) cout << ' ' << e;
    // cout << "\nedges";
    // for (const auto e : h.get_edges<1>()) cout << ' ' << e;
    cout << endl;

    cout << "bins";
    for (const auto b : h.bins) cout << ' ' << *b;
    cout << endl;
    cout << endl;
  }

  return 0;
}
