// Written by Ivan Pogrebnyak

#include <iostream>

#include <TH1.h>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

#include "binner_root.hh"

using std::cout;
using std::endl;
using namespace ivanp::root;

namespace ivanp { namespace root { namespace detail {

template <> struct bin_get<double> {
  inline const auto weight(const double& b) const noexcept { return b; }
};

}}}

int main()
{
  ivanp::binner<double, std::tuple<
    ivanp::axis_spec<ivanp::uniform_axis<double>,false,false>,
    ivanp::axis_spec<ivanp::uniform_axis<int>>,
    ivanp::axis_spec<ivanp::uniform_axis<float>,false,false>
  >> hist( {3,0,10}, {5,-2,3}, {2,0,5} );

  for (auto& bin : hist.bins()) {
    static int i = 0;
    bin = ++i;
  }

  const auto h1 = ivanp::slice<1>(hist,std::index_sequence<1,2,0>{});

  for (const auto& h : h1) {
    const auto* th = to_root(h,"hist");

    test(th->GetName())
    test(th->GetNbinsX())
    for (int i=0, n=th->GetNbinsX()+2; i<n; ++i)
      cout << ' ' << th->GetBinContent(i);
    cout << endl;

    cout << endl;
  }

  return 0;
}
