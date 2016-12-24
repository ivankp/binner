#include <type_traits>

#ifdef _CF
#error In __FILE__: cannot define macro _CF
#endif
#define _CF const final
#ifdef _CFN
#error In __FILE__: cannot define macro _CFN
#endif
#define _CFN const final noexcept
#ifdef _CFNN
#error In __FILE__: cannot define macro _CFNN
#endif
#define _CFNN(expr) const final noexcept(noexcept(expr))

namespace ivanp {

template <typename T, bool Scalar = std::is_scalar<T>::value>
struct const_ref_if_not_scalar {
  using type = std::add_lvalue_reference_t<std::add_const_t<T>>;
};
template <typename T>
struct const_ref_if_not_scalar<T,true> {
  using type = T;
};
template <typename T>
using const_ref_if_not_scalar_t = typename const_ref_if_not_scalar<T>::type;

enum class axis_overflow {
  both  = 0,
  none  = 1,
  under = 2,
  over  = 3
};

namespace detail {
  template <axis_overflow Overflow, typename SizeType=unsigned>
  struct overflow_offset { };
#define def_overflow_offset(T,N,L,U) \
  template <typename SizeType> \
  struct overflow_offset<axis_overflow::T,SizeType> { \
    static constexpr SizeType nbins = N; \
    static constexpr SizeType lower = L; \
    static constexpr SizeType upper = U; \
  };

  def_overflow_offset(both, 1,1,0)
  def_overflow_offset(none,-1,0,1)
  def_overflow_offset(under,0,1,0)
  def_overflow_offset(over, 0,0,1)
}

// Abstract Axis ====================================================

template <typename EdgeType>
class abstract_axis {
public:
  using size_type = unsigned;
  using edge_type = EdgeType;

  virtual axis_overflow overflow() const noexcept = 0;
  inline bool has_underflow() const noexcept {
    return (overflow()==both || overflow()==under);
  }
  inline bool has_overflow() const noexcept {
    return (overflow()==both || overflow()==over);
  }

  virtual size_type nbins () const = 0;
  virtual size_type nedges() const = 0;

  virtual size_type vfind_bin(edge_type x) const = 0;

  virtual edge_type edge   (size_type i) const = 0;
  virtual edge_type edge_at(size_type i) const = 0;
  virtual edge_type lower() const = 0;
  virtual edge_type lower(size_type bin) const = 0;
  virtual edge_type lower_at(size_type bin) const = 0;
  virtual edge_type upper() const = 0;
  virtual edge_type upper(size_type bin) const = 0;
  virtual edge_type upper_at(size_type bin) const = 0;
};

// Container Axis ===================================================

template <typename Container, axis_overflow Overflow = axis_overflow::both>
class container_axis: public abstract_axis<Container::value_type> {
public:
  using abstract_type = abstract_axis<Container::value_type>;
  using container_type = Container;
  static constexpr axis_overflow overflow_flag = Overflow;

private:
  container_type _edges;

public:
  container_axis(): _edges() { }
  // TODO: add all constructors

  inline axis_overflow overflow() _CFN { return Overflow; }

  inline size_type nedges() _CFNN(_edges.size()) { return _edges.size(); }
  inline size_type nbins () _CFNN(_edges.size()) { return _edges.size()
    + detail::overflow_offset<Overflow,size_type>::nbins;
  }

  inline edge_type edge   (size_type i) _CFN { return _edges[i]; }
  inline edge_type edge_at(size_type i) _CFNN(_edges.at(i)) {
    return _edges.at(i);
  }
  inline edge_type lower() _CFNN(_edges.front()) { return _edges.front(); }
  inline edge_type lower(size_type bin) _CFN {
    return _edges[bin-detail::overflow_offset<Overflow,size_type>::lower];
  }
  inline edge_type lower_at(size_type bin) _CFNN(_edges.at(bin)) {
    // TODO: better exceptions
    return _edges.at(bin-detail::overflow_offset<Overflow,size_type>::lower);
  }
  inline edge_type upper() _CFNN(_edges.back()) { return _edges.back(); }
  inline edge_type upper(size_type bin) _CFN {
    return _edges[bin+detail::overflow_offset<Overflow,size_type>::upper];
  }
  inline edge_type upper_at(size_type bin) _CFNN(_edges.at(bin)) {
    return _edges.at(bin+detail::overflow_offset<Overflow,size_type>::upper);
  }

  inline const container_type& edges() const { return _edges; }

  inline size_type vfind_bin(edge_type x) _CF { return find_bin(x); }

  template <typename T>
  size_type find_bin(const T& x) const noexcept {
    return std::distance(
      std::upper_bound(_edges.begin(), _edges.end(), x), _edges.begin()
    ) - detail::overflow_offset<Overflow>::upper;
    // binner desides what to do with out-of-range bins
  }

};

// Constexpr Axis ===================================================

template <typename T, axis_overflow Overflow = axis_overflow::both>
class const_axis: public abstract_axis<T> {

};

// ==================================================================
} // end namespace ivanp

#undef _CF
#undef _CFN
#undef _CFNN
