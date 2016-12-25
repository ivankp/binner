#include <algorithm>
#include <utility>
#include <type_traits>
#include <initializer_list>

#define test(var) \
  std::cout << "\033[36m" << #var << "\033[0m = " << var << std::endl;

#ifdef _CF
#error In __FILE__: cannot define macro _CF
#endif
#define _CF const final
#ifdef _CNF
#error In __FILE__: cannot define macro _CNF
#endif
#define _CNF const noexcept final
#ifdef _CNFN
#error In __FILE__: cannot define macro _CNFN
#endif
#define _CNFN(expr) const noexcept(noexcept(expr)) final

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
    return (overflow()==axis_overflow::both
         || overflow()==axis_overflow::under);
  }
  inline bool has_overflow() const noexcept {
    return (overflow()==axis_overflow::both
         || overflow()==axis_overflow::over);
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
class container_axis: public abstract_axis<typename Container::value_type> {
public:
  using base_type = abstract_axis<typename Container::value_type>;
  using size_type = typename base_type::size_type;
  using edge_type = typename base_type::edge_type;
  using container_type = Container;
  static constexpr axis_overflow overflow_flag = Overflow;

private:
  container_type _edges;

public:
  container_axis() = default;
  ~container_axis() = default;

  container_axis(const container_type& edges): _edges(edges) {
    std::cout << "const container_type&" << std::endl;
  }
  container_axis(container_type&& edges): _edges(std::move(edges)) {
    std::cout << "container_type&&" << std::endl;
  }
  container_axis(const container_axis& axis): _edges(axis._edges) {
    std::cout << "const container_axis&" << std::endl;
  }
  container_axis(container_axis&& axis): _edges(std::move(axis._edges)) {
    std::cout << "container_axis&&" << std::endl;
  }

  container_axis& operator=(const container_type& edges) {
    std::cout << "= const container_type&" << std::endl;
    _edges = edges;
    return *this;
  }
  container_axis& operator=(container_type&& edges) {
    std::cout << "= container_type&&" << std::endl;
    _edges = std::move(edges);
    return *this;
  }
  container_axis& operator=(const container_axis& axis) {
    std::cout << "= const container_axis&" << std::endl;
    _edges = axis._edges;
    return *this;
  }
  container_axis& operator=(container_axis&& axis) {
    std::cout << "= container_axis&&" << std::endl;
    _edges = std::move(axis._edges);
    return *this;
  }

  inline axis_overflow overflow() _CNF { return Overflow; }

  inline size_type nedges() _CNFN(_edges.size()) { return _edges.size(); }
  inline size_type nbins () _CNFN(_edges.size()) { return _edges.size()
    + detail::overflow_offset<Overflow,size_type>::nbins;
  }

  inline edge_type edge   (size_type i) _CNF { return _edges[i]; }
  inline edge_type edge_at(size_type i) _CNFN(_edges.at(i)) {
    return _edges.at(i);
  }
  inline edge_type lower() _CNFN(_edges.front()) { return _edges.front(); }
  inline edge_type lower(size_type bin) _CNF {
    return _edges[bin-detail::overflow_offset<Overflow,size_type>::lower];
  }
  inline edge_type lower_at(size_type bin) _CNFN(_edges.at(bin)) {
    // TODO: better exceptions
    return _edges.at(bin-detail::overflow_offset<Overflow,size_type>::lower);
  }
  inline edge_type upper() _CNFN(_edges.back()) { return _edges.back(); }
  inline edge_type upper(size_type bin) _CNF {
    return _edges[bin+detail::overflow_offset<Overflow,size_type>::upper];
  }
  inline edge_type upper_at(size_type bin) _CNFN(_edges.at(bin)) {
    return _edges.at(bin+detail::overflow_offset<Overflow,size_type>::upper);
  }

  inline const container_type& edges() const { return _edges; }

  inline size_type vfind_bin(edge_type x) _CF { return find_bin(x); }

  template <typename T>
  size_type find_bin(const T& x) const noexcept {
    return std::distance(
      _edges.begin(), std::upper_bound(_edges.begin(), _edges.end(), x)
    ) - detail::overflow_offset<Overflow>::upper;
    // output aligned but not necessarily in-range indices
    // binner desides what to do with out-of-range bins
  }

  template <typename T>
  inline size_type operator[](const T& x) const noexcept {
    return find_bin(x);
  }

};

// Constexpr Axis ===================================================

template <typename EdgeType, axis_overflow Overflow = axis_overflow::both>
class const_axis: public abstract_axis<EdgeType> {

};

// Uniform Axis =====================================================

template <typename EdgeType, axis_overflow Overflow = axis_overflow::both>
class uniform_axis: public abstract_axis<EdgeType> {

};

// Shared Axis ======================================================

template <typename EdgeType, typename Ref = const abstract_axis<EdgeType>*>
class ref_axis: public abstract_axis<EdgeType> {
public:
  using base_type = abstract_axis<EdgeType>;
  using size_type = typename base_type::size_type;
  using edge_type = typename base_type::edge_type;
  using axis_ref  = Ref;

private:
  axis_ref _ref;

public:
  ref_axis() = default;
  ~ref_axis() = default;

  ref_axis(axis_ref ref): _ref(ref) { }
  ref_axis& operator=(axis_ref ref) {
    _ref = ref;
    return *this;
  }

  inline axis_overflow overflow() _CNF { return _ref->overflow(); }

  inline size_type nedges() _CF { return _ref->nedges(); }
  inline size_type nbins () _CF { return _ref->nbins (); }

  inline size_type vfind_bin (edge_type x) _CF   { return _ref->vfind_bin(x); }
  inline size_type  find_bin (edge_type x) const { return _ref->vfind_bin(x); }
  inline size_type operator[](edge_type x) const { return _ref->vfind_bin(x); }

  inline edge_type edge   (size_type i) _CF { return _ref->edge(i); }
  inline edge_type edge_at(size_type i) _CF { return _ref->edge_at(i); }
  inline edge_type lower() _CF { return _ref->lower(); }
  inline edge_type lower(size_type bin) _CF { return _ref->lower(bin); }
  inline edge_type lower_at(size_type bin) _CF { return _ref->lower_at(bin); }
  inline edge_type upper() _CF { return _ref->upper(); }
  inline edge_type upper(size_type bin) _CF { return _ref->upper(bin); }
  inline edge_type upper_at(size_type bin) _CF { return _ref->upper_at(bin); }

};

// ==================================================================
} // end namespace ivanp

#undef _CF
#undef _CNF
#undef _CNFN
