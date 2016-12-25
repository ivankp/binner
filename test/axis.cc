#include <iostream>
#include <vector>
#include <array>

#include <axis.hh>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
  ivanp::container_axis<std::vector<double>> a({1.,2.,3.,4.});
  ivanp::container_axis<std::vector<double>> a2(a);
  // ivanp::container_axis<std::array<double,4>> a({1.,2.,3.,4.});

  test( a2[2.5] )

  return 0;
}
