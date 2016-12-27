#ifndef IVANP_AXIS_HH
#define IVANP_AXIS_HH

#include <algorithm>
#include <utility>
#include <stdexcept>
#include <sstream>

#include "type_traits.hh"

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
#ifdef _CNN
#error In __FILE__: cannot define macro _CNN
#endif
#define _CNN(expr) const noexcept(noexcept(expr))

namespace ivanp {

// INFO =============================================================

// all axes assume bin index in [0,n+1], where n is the number of edges

// Abstract Axis ====================================================

template <typename EdgeType>
class abstract_axis {
public:
  using size_type = unsigned;
  using edge_type = EdgeType;
  using edge_ltype = const_ref_if_not_scalar_t<edge_type>;

  virtual size_type nbins () const = 0;
  virtual size_type nedges() const = 0;

  virtual size_type vfind_bin(edge_ltype x) const = 0;

  virtual edge_ltype edge(size_type i) const = 0;
  virtual edge_ltype min() const = 0;
  virtual edge_ltype max() const = 0;
  virtual edge_ltype lower(size_type bin) const = 0;
  virtual edge_ltype upper(size_type bin) const = 0;

  inline bool check_edge(size_type i) const { return (i < nedges()); }
  inline void check_edge_throw(size_type i) const {
    if (!check_edge(i)) {
      std::ostringstream ss("Axis edge index ");
      ss << i << " >= " << nedges();
      throw std::range_error(ss.str());
    }
  }
  inline bool check_bin(size_type bin) const { return (bin < nbins()); }
  inline void check_bin_throw(size_type bin) const {
    if (!check_bin(bin)) {
      std::ostringstream ss("Axis bin index ");
      ss << bin << " >= " << nbins();
      throw std::range_error(ss.str());
    }
  }
};

// Container Axis ===================================================

template <typename Container>
class container_axis: public abstract_axis<typename Container::value_type> {
public:
  using base_type = abstract_axis<typename Container::value_type>;
  using size_type = typename base_type::size_type;
  using edge_type = typename base_type::edge_type;
  using edge_ltype = typename base_type::edge_ltype;
  using container_type = Container;

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

  inline size_type nedges() _CNFN(_edges.size()) { return _edges.size(); }
  inline size_type nbins () _CNFN(_edges.size()) { return _edges.size()-1; }

  inline edge_ltype edge(size_type i) _CNF { return _edges[i]; }

  inline edge_ltype min() _CNN(_edges.front()) { return _edges.front(); }
  inline edge_ltype max() _CNN(_edges.back()) { return _edges.back(); }

  inline edge_ltype lower(size_type bin) _CNF { return _edges[bin-1]; }
  inline edge_ltype upper(size_type bin) _CNF { return _edges[bin]; }

  template <typename T>
  size_type find_bin(const T& x) const noexcept {
    return std::distance(
      _edges.begin(), std::upper_bound(_edges.begin(), _edges.end(), x)
    );
  }
  inline size_type vfind_bin(edge_ltype x) _CF { return find_bin(x); }

  template <typename T>
  inline size_type operator[](const T& x) const noexcept {
    test(x) // TODO: called twice!!! <---------
    return find_bin(x);
  }

  inline const container_type& edges() const { return _edges; }

};

// Constexpr Axis ===================================================

template <typename EdgeType>
class const_axis: public abstract_axis<EdgeType> {

};

// Uniform Axis =====================================================

template <typename EdgeType>
class uniform_axis: public abstract_axis<EdgeType> {
public:
  using base_type = abstract_axis<EdgeType>;
  using size_type = typename base_type::size_type;
  using edge_type = typename base_type::edge_type;
  using edge_ltype = typename base_type::edge_ltype;

private:
  size_type _nbins;
  edge_type _min, _max;

public:
  uniform_axis() = default;
  ~uniform_axis() = default;
  uniform_axis(size_type nbins, edge_ltype min, edge_ltype max)
  : _nbins(nbins), _min(min), _max(max) { }
  uniform_axis(const uniform_axis& axis)
  : _nbins(axis._nbins), _min(axis._min), _max(axis._max) { }
  uniform_axis& operator=(const uniform_axis& axis) {
    _nbins = axis._nbins;
    _min = axis._min;
    _max = axis._max;
    return *this;
  }

  inline size_type nbins () _CNF { return _nbins; }
  inline size_type nedges() _CNF { return _nbins+1; }

  inline edge_ltype edge(size_type i) _CNF {
    // TODO: test
    const auto width = (_max - _min)/_nbins;
    return _min + i*width;
  }

  inline edge_ltype min() const noexcept { return _min; }
  inline edge_ltype max() const noexcept { return _max; }

  inline edge_ltype lower(size_type bin) _CNF { return edge(bin-1); }
  inline edge_ltype upper(size_type bin) _CNF { return edge(bin); }

  template <typename T>
  size_type find_bin(const T& x) const noexcept {
    if (x < _min) return 0;
    if (!(x < _max)) return _nbins+1;
    return _nbins*(x-_min)/(_max-_min) + 1;
  }

  inline size_type vfind_bin(edge_ltype x) _CNF { return find_bin(x); }

  template <typename T>
  inline size_type operator[](const T& x) const noexcept {
    return find_bin(x);
  }

  // TODO: exact edge calculation

};

// Shared Axis ======================================================

template <typename EdgeType, typename Ref = const abstract_axis<EdgeType>*>
class ref_axis: public abstract_axis<EdgeType> {
public:
  using base_type = abstract_axis<EdgeType>;
  using size_type = typename base_type::size_type;
  using edge_type = typename base_type::edge_type;
  using edge_ltype = typename base_type::edge_ltype;
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
  ref_axis(const ref_axis& axis): _ref(axis._ref) { }
  ref_axis(ref_axis&& axis): _ref(std::move(axis._ref)) { }
  ref_axis& operator=(const ref_axis& axis) {
    _ref = axis._ref;
    return *this;
  }
  ref_axis& operator=(ref_axis&& axis) {
    _ref = std::move(axis._ref);
    return *this;
  }

  inline size_type nedges() _CF { return _ref->nedges(); }
  inline size_type nbins () _CF { return _ref->nbins (); }

  inline size_type vfind_bin (edge_ltype x) _CF   { return _ref->vfind_bin(x); }
  inline size_type  find_bin (edge_ltype x) const { return _ref->vfind_bin(x); }
  inline size_type operator[](edge_ltype x) const { return _ref->vfind_bin(x); }

  inline edge_ltype edge(size_type i) _CF { return _ref->edge(i); }
  inline edge_ltype min() _CF { return _ref->min(); }
  inline edge_ltype max() _CF { return _ref->max(); }
  inline edge_ltype lower(size_type bin) _CF { return _ref->lower(bin); }
  inline edge_ltype upper(size_type bin) _CF { return _ref->upper(bin); }

};

// ==================================================================
} // end namespace ivanp

#undef _CF
#undef _CNF
#undef _CNN
#undef _CNFN

#endif
