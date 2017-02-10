#ifndef IVANP_SEQUENCE_TRAITS_HH
#define IVANP_SEQUENCE_TRAITS_HH

namespace ivanp {

// increment ********************************************************

template <typename Seq, typename Seq::value_type Inc>
struct increment_integer_sequence;
template <typename T, T... I, T Inc>
struct increment_integer_sequence<std::integer_sequence<T,I...>, Inc> {
  using type = std::integer_sequence<T,(I+Inc)...>;
};

// range ************************************************************

template <typename T, size_t A, size_t B>
using make_integer_range = typename
  increment_integer_sequence<std::make_integer_sequence<T,B-A>,A>::type;
template <size_t A, size_t B>
using make_index_range = typename
  increment_integer_sequence<std::make_index_sequence<B-A>,A>::type;

// subsequence ******************************************************

template <size_t N, typename Seq>
struct right_subseq {
  template <size_t _N, typename _Seq> struct impl;
  template <size_t _N, typename T, T I1, T... II>
  struct impl<_N,std::integer_sequence<T,I1,II...>> {
    using type = typename
      right_subseq<_N-1,std::integer_sequence<T,II...>>::type;
  };
  using type = typename impl<N,Seq>::type;
};
template <typename Seq>
struct right_subseq<0,Seq> { using type = Seq; };
template <size_t N, typename Seq>
using right_subseq_t = typename right_subseq<N,Seq>::type;

template <typename Seq> struct seq_front;
template <typename T, T I1, T... II>
struct seq_front<std::integer_sequence<T,I1,II...>> {
  static constexpr T value = I1;
};

// sequence element *************************************************

template <size_t I, typename Seq>
struct seq_element {
  static constexpr auto value = seq_front<right_subseq_t<I,Seq>>::value;
};

// invert ***********************************************************

template <typename S, typename I
  = std::make_integer_sequence<typename S::value_type, S::size()>
> struct seq_inv;
template <typename T, T... S, T... I>
class seq_inv<std::integer_sequence<T,S...>,std::integer_sequence<T,I...>> {
  using seq = std::integer_sequence<T,S...>;
  typedef char arr[seq::size()];

  constexpr static auto inv(T j) {
    arr aseq = { seq_element<I,seq>::value... };
    arr ainv = { I... };
    for (T i=0; i<seq::size(); ++i) ainv[aseq[i]] = i;
    return ainv[j];
  }

public:
  using type = std::integer_sequence<T,inv(I)...>;
};
template <typename S> using seq_inv_t = typename seq_inv<S>::type;

// ******************************************************************

} // end namespace ivanp

#endif
