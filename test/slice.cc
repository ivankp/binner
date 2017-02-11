// Written by Ivan Pogrebnyak

#include <iostream>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

#include "slice.hh"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
  ivanp::binner<double, std::tuple<
    ivanp::axis_spec<ivanp::uniform_axis<double>,false,false>,
    ivanp::axis_spec<ivanp::uniform_axis<int>>
  >> hist( {3,0,10}, {5,-2,3} );

  for (auto& bin : hist.bins()) {
    static int i = 0;
    bin = ++i;
  }

  const auto h1 = ivanp::slice(hist);
  // const auto h1 = ivanp::slice(hist,std::index_sequence<1,0>{});

  for (const auto& h : h1) {
    cout << '[' << std::get<0>(h.ranges)[0] << ','
         << std::get<0>(h.ranges)[1] << ')' << endl;

    cout << "nbins = " << h.bins.size() << endl;

    cout << "edges";
    for (const auto e : *std::get<0>(h.edges)) cout << ' ' << e;
    cout << endl;

    cout << "bins";
    for (const auto b : h.bins) cout << ' ' << *b;
    cout << endl;
    cout << endl;
  }

  return 0;
}
