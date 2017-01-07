#ifndef IVANP_TYPE_TRAITS_HH
#define IVANP_TYPE_TRAITS_HH

#include <type_traits>

namespace ivanp {

template <typename New, typename Old> using replace_t = New;

// boolean compositing **********************************************

template <bool...> struct bool_sequence {};

template <bool... B>
using bool_and = std::is_same< bool_sequence< B... >,
                               bool_sequence< ( B || true )... > >;
template <bool... B>
using bool_or = std::integral_constant< bool, !bool_and< !B... >::value >;

// ******************************************************************

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

// IS ***************************************************************

template <typename T> struct is_std_array: std::false_type { };
template <typename T, size_t N>
struct is_std_array<std::array<T,N>>: std::true_type { };

template <typename T> struct is_std_vector: std::false_type { };
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T,Alloc>>: std::true_type { };

template <typename T> struct is_integer_sequence: std::false_type { };
template <typename T, T... Ints>
struct is_integer_sequence<std::integer_sequence<T,Ints...>>: std::true_type { };

} // end namespace ivanp

#endif
