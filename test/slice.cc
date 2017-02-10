// Written by Ivan Pogrebnyak

#include <iostream>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

#include "slice.hh"

int main(int argc, char* argv[])
{
  ivanp::binner<double, std::tuple<
    ivanp::axis_spec<ivanp::uniform_axis<double>,false,false>,
    ivanp::axis_spec<ivanp::uniform_axis<int>>
  >> hist( {3,0,10}, {5,-2,3} );

  // ivanp::slice(hist);
  ivanp::slice(hist,std::index_sequence<1,0>{});

  return 0;
}
