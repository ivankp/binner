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
  using nover = std::integral_constant<axis_size_type,Uf+Of>;
};

template <typename Bin,
          typename AxesSpecs = std::tuple<axis_spec<uniform_axis<double>>>,
          typename Filler = default_bin_filler<Bin>,
          typename Container = std::vector<Bin>>
class binner {
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
  using size_type = ivanp::axis_size_type;
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

  // TODO: std::array _make_axis
  template <size_t I, typename T, size_t N, size_t... A>
  inline axis_type<I> _make_axis(
    std::index_sequence<A...>,
    const std::array<T,N>& args
  ) { return axis_type<I>(args); }

  template <typename... Axes, size_t... I>
  inline binner(std::index_sequence<I...> is, std::tuple<Axes...> t)
  : _axes{_make_axis<I>(
      std::make_index_sequence<
        std::tuple_size<
          std::decay_t<std::tuple_element_t<I,decltype(t)>>
        >::value
      >(), std::get<I>(t) )...}, _bins()
  {
    // TODO: specialize instead of using runtime if
    // http://stackoverflow.com/q/41473675/2640636
    if (is_std_vector<container_type>::value)
      _bins.resize(nbins_total());
    if (std::is_arithmetic<typename container_type::value_type>::value)
      for (auto& b : _bins) b = 0;
  }

  template <size_t I>
  inline std::enable_if_t<I!=0,size_type> nbins_total_impl() const noexcept {
    return ( axis<I>().nbins() + axis_spec<I>::nover::value
      ) * nbins_total_impl<I-1>();
  }
  template <size_t I>
  inline std::enable_if_t<I==0,size_type> nbins_total_impl() const noexcept {
    return ( axis<0>().nbins() + axis_spec<0>::nover::value );
  }

  template <size_t I=0, typename T, typename... Args>
  inline size_type find_bin_impl(const T& x, const Args&... args) const {
    return axis<I>().find_bin(x)
      + (axis<I>().nbins() - !axis_spec<I>::under::value)
      * find_bin_impl<I+1>(args...);
  }
  template <size_t I=0, typename T>
  inline size_type find_bin_impl(const T& x) const {
    return axis<I>().find_bin(x) - !axis_spec<I>::under::value;
  }

public:
  binner() = delete;
  ~binner() = default;

  template <typename... Axes, typename = std::enable_if_t<
    (sizeof...(Axes)==naxes) &&
    !bool_or<is_integer_sequence<Axes>::value...>::value
  >>
  inline binner(Axes&&... axes): binner(
    std::index_sequence_for<Axes...>(),
    std::forward_as_tuple(std::forward<Axes>(axes)...)
  ) { }

  template <unsigned I=0>
  constexpr const axis_type<I> axis() const noexcept {
    return std::get<I>(_axes);
  }

  size_type nbins_total() const noexcept {
    return nbins_total_impl<naxes-1>();
  }

  constexpr const container_type& bins() const noexcept { return _bins; }
  constexpr container_type& bins() noexcept { return _bins; }

  inline size_type fill_bin(size_type bin) {
    ++_bins[bin];
    return bin;
  }
  template <typename W>
  inline size_type fill_bin(size_type bin, W&& weight) {
    _bins[bin] += std::forward<W>(weight);
    return bin;
  }

  template <typename... Args>
  size_type find_bin(const Args&... args) const {
    static_assert(sizeof...(args)==naxes);
    return find_bin_impl(args...);
  }

  template <typename... Args>
  size_type fill(const Args&... args) {
    static_assert(sizeof...(args)==naxes);
    return fill_bin(find_bin_impl(args...));
  }
};

} // end namespace ivanp

#endif
