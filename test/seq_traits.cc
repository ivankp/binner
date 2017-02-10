// Written by Ivan Pogrebnyak

#include <iostream>
#include <utility>

#include <boost/type_index.hpp>

#include "sequence_traits.hh"

using std::cout;
using std::endl;
using namespace ivanp::seq;

template <typename T> struct prt_type { };
template <typename T>
std::ostream& operator<<(std::ostream& os, prt_type<T>) {
  os << "\033[1;35m" << boost::typeindex::type_id<T>().pretty_name()
     << "\033[0m";
  return os;
}

int main(int argc, char* argv[])
{
  cout << std::boolalpha;

  using s1 = std::index_sequence<0,3,1,2>;
  using s2 = std::integer_sequence<int,1,2,0>;
  using i1 = inverse_t<s1>;
  using i2 = inverse_t<s2>;

  cout << "Original: " << prt_type<s1>{} << endl;
  cout << "Inverse : " << prt_type<i1>{} << endl;
  cout << endl;
  cout << "Original: " << prt_type<s2>{} << endl;
  cout << "Inverse : " << prt_type<i2>{} << endl;

  return 0;
}
