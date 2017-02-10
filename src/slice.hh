#ifndef IVANP_BINNER_SLICE_HH
#define IVANP_BINNER_SLICE_HH

#include "binner.hh"

namespace ivanp {

namespace detail { namespace slice {

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
    std::cout << "++"; // test
    ++_ii[0];
    for (size_type i=0; i<N; ++i) {
      if (_ii[i] == _nn[i]) {
        _ii[i] = 0;
        _next = true;
        if (i==N-1) _done = true;
        else ++_ii[i+1];
      }
      std::cout << ' ' << _ii[i]; // test
    }
    std::cout << std::endl; // test
    return *this;
  }

  inline operator bool() const noexcept { return _done; }
  inline bool next() const noexcept { return _next; }
  inline const std::array<size_type,N>& ii() const noexcept { return _ii; }
};

template <typename... A, bool... U, size_t... I>
inline auto make_ranges(
  const std::tuple<A...>& axes,
  const std::array<ivanp::axis_size_type,sizeof...(I)>& ii,
  std::integer_sequence<bool,U...>,
  std::index_sequence<I...>
) -> std::tuple<std::array<
  typename std::tuple_element_t<I,std::tuple<std::decay_t<A>...>>::edge_type,
  2>...>
{
  using under = std::integer_sequence<bool,!U...>;
  constexpr auto ImA = sizeof...(I)-sizeof...(A);
  return { {
    std::get<I>(axes).lower( std::get<I+ImA>(ii)+seq_element<I,under>::value ),
    std::get<I>(axes).upper( std::get<I+ImA>(ii)+seq_element<I,under>::value )
  }... };
}

} }

// template <typename T> struct test_type;

template <size_t D=1, // slicing into chunks of D dimensions
          typename Bin,
          typename... Ax,
          typename Container,
          typename Filler,
          size_t... I
>
void slice(
  const binner<Bin,std::tuple<Ax...>,Container,Filler>& hist,
  std::index_sequence<I...>
) {
  static_assert( sizeof...(I) == sizeof...(Ax),
    "the number of indices must match the number of axes");
  // static_assert( D,
  //   "at least 1 dimension must be unsliced");
  static_assert( D <= sizeof...(I),
    "cannot leave more than total number of dimensions unsliced");
  using namespace ivanp::detail::slice;

  using tail = make_index_range<D,sizeof...(I)>;

  constexpr auto nover = std::make_tuple(Ax::nover::value...);
  const auto& axes = hist.axes();

  const auto reordered_axes = std::tie(as_const(std::get<I>(axes))...);
  using under = std::integer_sequence<bool,
    std::tuple_element_t<I,std::tuple<Ax...>>::under::value...>;

  counter<tail::size()> cnt(
    std::make_tuple(
      ( std::get<I>(axes).nbins() + std::get<I>(nover) )...
    ), tail{}
  ); // construct counter

  for ( ; !cnt; ++cnt ) {
    const auto ranges = make_ranges(
      reordered_axes, cnt.ii(), under{}, tail{}
    );
    test( std::tuple_size<decltype(ranges)>::value ) // test
    test( std::get<0>(ranges)[0] ) // test
    test( std::get<0>(ranges)[1] ) // test
  }
  
}

template <size_t D=1, // slicing into chunks of D dimensions
          typename Bin,
          typename... Ax,
          typename Container,
          typename Filler
>
inline void slice( // overload for default axis order
  const binner<Bin,std::tuple<Ax...>,Container,Filler>& hist
) { return slice<D>(hist,std::index_sequence_for<Ax...>{}); }


} // end namespace ivanp

#endif
