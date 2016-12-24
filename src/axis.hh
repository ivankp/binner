#include <type_traits>

namespace ivanp {

enum class axis_overflow {
  both  = 0,
  none  = 1,
  under = 2,
  over  = 3
};

namespace detail {
/*
  template <typename SizeType, axis_overflow Overflow> struct n_extra_bins { };
#define def_n_extra_bins(A,B) \
  template <typename SizeType> \
  struct n_extra_bins<SizeType,axis_overflow::A> { \
    static constexpr SizeType value = B; \
  };

  def_n_extra_bins(both,1)
  def_n_extra_bins(none,-1)
  def_n_extra_bins(under,0)
  def_n_extra_bins(over,0)
*/
}

template <typename EdgeType>
class abstract_axis {
public:
  using size_type = unsigned;
  using edge_type = EdgeType;

  virtual axis_overflow overflow() const = 0;

  virtual size_type nbins () const = 0;
  virtual size_type nedges() const = 0;

  virtual size_type find_bin() const = 0;

  virtual edge_type lower() const = 0;
  virtual edge_type lower(size_type i) const = 0;
  virtual edge_type upper() const = 0;
  virtual edge_type upper(size_type i) const = 0;
};

#define overflow_enable(value) \
  template <axis_overflow Overflow_ = Overflow> \
  std::enable_if_t<Overflow_==axis_overflow::value,size_type>

template <typename Container, axis_overflow Overflow = axis_overflow::both>
class array_axis : public abstract_axis<Container::value_type> {
public:
  using abstract_type = abstract_axis<Container::value_type>;
  using container_type = Container;

private:
  container_type edges;

public:
  constexpr array_axis(): edges() { }

  axis_overflow overflow() const final { return Overflow; }

  size_type nedges() const final { return edges.size(); }
  overflow_enable(both)  nbins() const final { return edges.size()+1; }
  overflow_enable(none)  nbins() const final { return edges.size()-1; }
  overflow_enable(under) nbins() const final { return edges.size();   }
  overflow_enable(over)  nbins() const final { return edges.size();   }
};

// Axis traits ======================================================

template <typename Axis> struct overflow_trait { };
template <typename Container, axis_overflow Overflow>
struct overflow_trait<array_axis<Container,Overflow>> {
  static constexpr axis_overflow value = Overflow;
};

}
