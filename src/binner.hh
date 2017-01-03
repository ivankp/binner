// Written by Ivan Pogrebnyak

#ifndef IVANP_BINNER_HH
#define IVANP_BINNER_HH

#include <tuple>

#include "axis.hh"
#include "default_bin_filler.hh"

namespace ivanp {

template <typename Axis, bool Uf=true, bool Of=true>
struct axis_spec {
  using axis  = Axis;
  using under = std::integral_constant<bool,Uf>;
  using over  = std::integral_constant<bool,Of>;
};

template <typename Bin,
          typename AxesSpecs = std::tuple<axis_spec<uniform_axis<double>>>,
          typename Filler = default_bin_filler<Bin>,
          typename Container = std::vector<Bin>>
class binner {
  // http://stackoverflow.com/a/9123810/2640636
  template<typename T> struct strip_specs;
  template<typename... Axes>
  struct strip_specs<std::tuple<Axes...>> {
    using axes = std::tuple<typename Axes::axis...>;
  };
  using axes_tuple = typename strip_specs<AxesSpecs>::axes;

public:
  using bin_type = Bin;
  template <unsigned I>
  using axis_spec = std::tuple_element_t<I,AxesSpecs>;
  template <unsigned I>
  using axis_type = typename axis_spec<I>::axis;
  template <unsigned I>
  using edge_type = typename axis_type<I>::edge_type;
  using filler_type = Filler;
  using container_type = Container;
  static constexpr unsigned naxes = std::tuple_size<AxesSpecs>::value;

  static_assert(naxes>0);

private:
  axes_tuple _axes;
  container_type _bins;

  template <size_t I, typename... Args, size_t... A>
  inline axis_type<I> _make_axis(
    std::index_sequence<A...>,
    std::tuple<Args...> args
  ) { return axis_type<I>(std::get<A>(args)...); }

  template <typename... Axes, size_t... I>
  inline binner(std::index_sequence<I...> is, std::tuple<Axes...> t)
  : _axes{_make_axis<I>(
      std::make_index_sequence<
        std::tuple_size<
          std::decay_t<std::tuple_element_t<I,decltype(t)>>
        >::value
      >(), std::get<I>(t) )...}, _bins() { }

public:
  constexpr binner(): _axes(), _bins() { }
  ~binner() = default;

  template <typename... Axes,
            typename = std::enable_if_t<sizeof...(Axes)==naxes>>
  binner(Axes&&... axes): binner(
    std::index_sequence_for<Axes...>(),
    std::forward_as_tuple(std::forward<Axes>(axes)...)
  ) { }

  template <unsigned I=0>
  const axis_type<I> axis() const noexcept { return std::get<I>(_axes); }
};

} // end namespace ivanp

#endif
