#ifndef IVANP_BINNER_SLICE_HH
#define IVANP_BINNER_SLICE_HH

#include "binner.hh"
#include <memory>

namespace ivanp {

template <typename Axis>
auto make_shared_vector_of_edges(const Axis& axis) {
  using vec_t = std::vector<typename Axis::edge_type>;
  const auto n = axis.nedges();
  vec_t vec;
  vec.reserve(n);
  for (typename Axis::size_type i=0; i<n; ++i)
    vec.emplace_back(axis.edge(i));
  return std::make_shared<const vec_t>(std::move(vec));
}

namespace detail { namespace slice {

template <typename... A, size_t... I>
auto get_edges(const std::tuple<A...>& axes, std::index_sequence<I...>) {
  return std::make_tuple(make_shared_vector_of_edges( std::get<I>(axes) )...);
}

template <size_t N>
class counter {
  using size_type = ivanp::axis_size_type;

  std::array<size_type,N> _ii, _nn;
  bool _next, _done;

public:
  template <typename M, size_t... I>
  counter(const M& m, std::index_sequence<I...>) noexcept
  : _ii{zero(I)...}, _nn{std::get<I>(m)...},
    _next(false), _done(!sizeof...(I)) { }

  counter& operator++() noexcept {
    _next = false;
    ++_ii[0];
    for (size_type i=0; i<N; ++i) {
      if (_ii[i] == _nn[i]) {
        _ii[i] = 0;
        _next = true;
        if (i==N-1) _done = true;
        else ++_ii[i+1];
      }
    }
    return *this;
  }

  inline operator bool() const noexcept { return _done; }
  inline bool next() const noexcept { return _next; }
  inline const std::array<size_type,N>& ii() const noexcept { return _ii; }
  size_type size() const noexcept {
    size_type s = 1;
    for (const auto& n : _nn) s *= n;
    return s;
  }
  inline void reset() noexcept { _done = false; }
};

template <size_t I=0, size_t N>
inline std::enable_if_t<I==N-1, ivanp::axis_size_type> index(
  const std::array<const ivanp::axis_size_type*,N>& ii,
  const std::array<ivanp::axis_size_type,N>& nn
) noexcept {
  return *std::get<I>(ii);
}
template <size_t I=0, size_t N>
inline std::enable_if_t<I!=N-1, ivanp::axis_size_type> index(
  const std::array<const ivanp::axis_size_type*,N>& ii,
  const std::array<ivanp::axis_size_type,N>& nn
) noexcept {
  return *std::get<I>(ii) + std::get<I>(nn) * index<I+1>(ii,nn);
}

template <typename... A, bool... U, size_t... I>
auto make_ranges(
  const std::tuple<A...>& axes,
  const std::array<ivanp::axis_size_type,sizeof...(I)>& ii,
  std::integer_sequence<bool,U...>,
  std::index_sequence<I...>
) -> std::tuple<std::array<
  typename std::tuple_element_t<I,std::tuple<std::decay_t<A>...>>::edge_type,
  2>...>
{
  using namespace ivanp::seq;
  using under = std::integer_sequence<bool,!U...>;
  constexpr auto ImA = sizeof...(I)-sizeof...(A);
  return { {
    std::get<I>(axes).lower( std::get<I+ImA>(ii)+element<I,under>::value ),
    std::get<I>(axes).upper( std::get<I+ImA>(ii)+element<I,under>::value )
  }... };
}

template <typename T, size_t N1, size_t N2, size_t... I1, size_t... I2>
inline auto cat_cptr(
  const std::array<T,N1>& arr1,
  const std::array<T,N2>& arr2,
  std::index_sequence<I1...>,
  std::index_sequence<I2...>
) -> std::array<const T*, N1+N2> {
  static_assert(sizeof...(I1) == N1,"");
  static_assert(sizeof...(I2) == N2,"");
  return { &std::get<I1>(arr1)..., &std::get<I2>(arr2)... };
}

template <typename T, size_t N, size_t... I>
inline std::array<T,N> sort(std::array<T,N> arr, std::index_sequence<I...>) {
  static_assert(sizeof...(I) == N,"");
  return { std::get<I>(arr)... };
}

template <typename Bin, typename Ax, typename Head, typename Tail>
struct binner_slice;
template <typename Bin, typename... Ax, size_t... IH, size_t... IT>
struct binner_slice<Bin, const std::tuple<const Ax&...>,
  std::index_sequence<IH...>, std::index_sequence<IT...>
> {
  using axes_tuple = std::tuple<Ax...>;

  using ranges_type = std::tuple<std::array<
    typename std::tuple_element_t<IT,axes_tuple>::edge_type, 2>...>;
  using edges_type = std::tuple<
    std::shared_ptr<const std::vector<
      typename std::tuple_element_t<IH,axes_tuple>::edge_type
    >>...>;
  using bins_type = std::vector<const Bin*>;

  constexpr static auto n_dim = sizeof...(IH);
  constexpr static auto n_extra_dim = sizeof...(IT);

  ranges_type ranges;
  edges_type  edges;
  bins_type   bins;

  template <size_t I>
  const auto& get_edges() const noexcept { return *std::get<I>(edges); }

  template <typename... L>
  struct name_proxy {
    static_assert(sizeof...(L) == sizeof...(IT),"");
    const binner_slice *ptr;
    const std::tuple<L...> labels;

    template <size_t I=0>
    inline std::enable_if_t<I==sizeof...(IT), std::ostream>&
    impl(std::ostream& os) const { return os; }
    template <size_t I=0>
    inline std::enable_if_t<I!=sizeof...(IT), std::ostream>&
    impl(std::ostream& os) const {
      os << '_' << std::get<I>(labels)
         << '[' << std::get<I>(ptr->ranges)[0]
         << ',' << std::get<I>(ptr->ranges)[1] << ')';
      return impl<I+1>(os);
    }

    friend std::ostream& operator<<( std::ostream& os, const name_proxy& p)
    { return p.impl(os); }
  };
  template <typename... L>
  inline auto name(L&&... labels) const noexcept -> name_proxy<L...> {
    return { this, std::forward_as_tuple(std::forward<L>(labels)...) };
  }
};

} }

template <size_t D=1, // slicing into chunks of D dimensions
          typename Bin,
          typename... Ax,
          typename Container,
          typename Filler,
          size_t... I
>
auto slice(
  const binner<Bin,std::tuple<Ax...>,Container,Filler>& hist,
  std::index_sequence<I...>
) {
  static_assert( sizeof...(I) == sizeof...(Ax),
    "\033[33mthe number of indices must match the number of axes\033[0m");
  static_assert( D > 0,
    "\033[33mmust leave at least 1 dimension unsliced\033[0m");
  static_assert( D < sizeof...(I),
    "\033[33mnumber of unsliced dimensions must be less than total\033[0m");
  using namespace ivanp::detail::slice;

  using specs = std::tuple<Ax...>;
  using head = std::make_index_sequence<D>;
  using tail = seq::make_index_range<D,sizeof...(I)>;
  using inv  = seq::inverse_t<std::index_sequence<I...>>;

  using under = std::integer_sequence<bool,
    std::tuple_element_t<I,specs>::under::value...>;

  const auto& axes = hist.axes();
  const auto reordered_axes = std::tie(as_const(std::get<I>(axes))...);

  const auto reordered_nbins = make_array<ivanp::axis_size_type>(
    ( std::get<I>(axes).nbins()
      + std::tuple_element_t<I,specs>::nover::value )... );
  const auto nbins = sort(reordered_nbins, inv{});

  const auto edges = get_edges(reordered_axes, head{});

  counter<tail::size()> tcnt(reordered_nbins, tail{});
  counter<head::size()> hcnt(reordered_nbins, head{});
  const auto n_head_bins = hcnt.size();

  const auto ii = sort(
    cat_cptr(hcnt.ii(), tcnt.ii(),
      head{}, std::make_index_sequence<tail::size()>{}),
    inv{});

  using slice_t = binner_slice<Bin, decltype(reordered_axes), head, tail>;

  std::vector<slice_t> slices;
  slices.reserve(tcnt.size());

  const auto& hbins = hist.bins();

  for ( ; !tcnt; ++tcnt ) {
    std::vector<const Bin*> bins;
    bins.reserve(n_head_bins);
    for ( ; !hcnt; ++hcnt ) {
      bins.emplace_back(&hbins[index(ii,nbins)]);
    }
    hcnt.reset();

    slices.push_back({
      make_ranges( reordered_axes, tcnt.ii(), under{}, tail{} ),
      edges, std::move(bins) });
  }

  return slices;
}

template <size_t D=1, // slicing into chunks of D dimensions
          typename Bin,
          typename... Ax,
          typename Container,
          typename Filler
>
inline auto slice( // overload for default axis order
  const binner<Bin,std::tuple<Ax...>,Container,Filler>& hist
) { return slice<D>(hist,std::index_sequence_for<Ax...>{}); }

template <size_t D=1, // slicing into chunks of D dimensions
          size_t... I,
          typename Bin,
          typename... Ax,
          typename Container,
          typename Filler
>
inline auto slice( // overload for default axis order
  const binner<Bin,std::tuple<Ax...>,Container,Filler>& hist
) { return slice<D>(hist,std::index_sequence<I...>{}); }


} // end namespace ivanp

#endif
