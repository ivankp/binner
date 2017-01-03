// Written by Ivan Pogrebnyak

template <typename T>
class const_array {
  const T* p;
  unsigned n;
public:
  template <unsigned N>
  explicit constexpr const_array(const T(&a)[N]): p(a), n(N) { }

  constexpr unsigned size() const { return n; }
};

int main(int argc, char* argv[]) {

  // works
  static_assert(const_array<double>{{1.,2.,3.}}.size() == 3);

  // works
  constexpr const_array<double> a1((const double[]){1.,2.,3.});
  static_assert(a1.size() == 3);

  // doesn't compile
  // constexpr const_array<double> a2{{1.,2.,3.}};
  // static_assert(a2.size() == 3);
}
