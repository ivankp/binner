// Written by Ivan Pogrebnyak

#include <iostream>
#include <iomanip>

#include <TH1.h>
#include <TH2.h>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

#include "binner_root.hh"

using std::cout;
using std::endl;
using std::setw;
using namespace ivanp::root;

namespace ivanp { namespace root {
template <> struct bin_converter<double> {
  inline const auto weight(const double& b) const noexcept { return b; }
  // inline const auto  sumw2(const double& b) const noexcept { return 1; }
  // inline const auto    num(const double& b) const noexcept { return 2; }
};
}}

int main()
{
  ivanp::binner<double, std::tuple<
    ivanp::axis_spec<ivanp::uniform_axis<double>,false,false>,
    ivanp::axis_spec<ivanp::uniform_axis<int>>,
    ivanp::axis_spec<ivanp::uniform_axis<float>/*,false,false*/>
  >> hist( {3,0,10}, {5,-2,3}, {2,0,5} );

  for (auto& bin : hist.bins()) {
    static int i = 0;
    bin = ++i;
  }

  const auto h1 = ivanp::slice<2>(hist,std::index_sequence<1,2,0>{});

  cout.precision(3);

  for (const auto& h : h1) {
    const auto* th = to_root(h,"hist",0);

    test(th->ClassName())
    test(th->GetName())
    test(th->GetEntries())
    test(th->GetSize())
    test(th->GetNbinsX())
    test(th->GetNbinsY())
    for (int i=0, n=th->GetNbinsX()+2; i<n; ++i)
      cout << ' ' << setw(4) << th->GetBinContent(i);
    cout << endl;
    for (int i=0, n=th->GetNbinsX()+2; i<n; ++i)
      cout << ' ' << setw(4) << th->GetBinError(i);
    cout << endl;

    cout << endl;
  }

  return 0;
}
