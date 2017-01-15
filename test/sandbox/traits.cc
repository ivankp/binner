#include <iostream>
#include "type_traits.hh"

struct foo {
  void operator()(int i) { }
};

int main() {
  std::cout << ivanp::is_callable<foo,int>::value << std::endl;
  std::cout << ivanp::is_callable<foo,double>::value << std::endl;
  std::cout << ivanp::is_callable<foo,char[2]>::value << std::endl;
}
