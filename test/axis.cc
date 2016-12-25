#include <iostream>
#include <vector>

#include <axis.hh>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
  ivanp::container_axis<std::vector<double>> a({1.,2.,3.,4.});

  test( a[2.5] )

  return 0;
}
