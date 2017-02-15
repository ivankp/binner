#ifndef IVANP_BINNER_ROOT_HH
#define IVANP_BINNER_ROOT_HH

#include <sstream>

#include "slice.hh"

namespace ivanp {
namespace root {

#ifdef ROOT_TH1

template <typename A1>
TH1D* make_TH(const std::string& name, const std::tuple<A1>& axes) {
  const auto& a1 = std::get<0>(axes);
  if (a1.is_uniform()) {
    return new TH1D(name.c_str(),"",a1.nbins(),a1.min(),a1.max());
  } else {
    return new TH1D(name.c_str(),"",
      a1.nbins(),vector_of_edges<double>(a1).data());
  }
}

#endif

#ifdef ROOT_TH2

template <typename A1, typename A2>
TH2D* make_TH(const std::string& name, const std::tuple<A1,A2>& axes) {
  const auto& a1 = std::get<0>(axes);
  const auto& a2 = std::get<1>(axes);
  if (a1.is_uniform()) {
    if (a2.is_uniform()) {
      return new TH2D(name.c_str(),"",
        a1.nbins(),a1.min(),a1.max(), a2.nbins(),a2.min(),a2.max());
    } else {
      return new TH2D(name.c_str(),"",
        a1.nbins(),a1.min(),a1.max(),
        a2.nbins(),vector_of_edges<double>(a2).data());
    }
  } else {
    const auto e1 = vector_of_edges<double>(a1);
    if (a2.is_uniform()) {
      return new TH2D(name.c_str(),"",
        a1.nbins(),e1.data(), a2.nbins(),a2.min(),a2.max());
    } else {
      return new TH2D(name.c_str(),"",
        a1.nbins(),e1.data(), a2.nbins(),vector_of_edges<double>(a2).data());
    }
  }
}

#endif

#ifdef ROOT_TH3

template <typename A1, typename A2, typename A3>
TH3D* make_TH(const std::string& name, const std::tuple<A1,A2,A3>& axes) {
  const auto& a1 = std::get<0>(axes);
  const auto& a2 = std::get<1>(axes);
  const auto& a3 = std::get<2>(axes);
  if (a1.is_uniform() && a2.is_uniform() && a3.is_uniform()) {
    return new TH3D(name.c_str(),"",
      a1.nbins(),a1.min(),a1.max(),
      a2.nbins(),a2.min(),a2.max(),
      a3.nbins(),a3.min(),a3.max());
  } else {
    return new TH3D(name.c_str(),"",
      a1.nbins(),vector_of_edges<double>(a1).data(),
      a2.nbins(),vector_of_edges<double>(a2).data(),
      a3.nbins(),vector_of_edges<double>(a3).data());
  }
}

#endif

namespace detail {

template <typename Bin>
struct bin_get {
  inline const auto& weight(const Bin& b) const noexcept { return b.w; }
  inline const auto&  sumw2(const Bin& b) const noexcept { return b.w2; }
  inline const auto&    num(const Bin& b) const noexcept { return b.n; }
};

template <typename F, typename Bin>
class bin_get_traits {
  template <typename, typename = void>
  struct _has_weight : std::false_type { };
  template <typename T> struct _has_weight<T,
    void_t<decltype( std::declval<T>().weight(std::declval<Bin>()) )>
  > : std::true_type { };
  
  template <typename, typename = void>
  struct _has_sumw2 : std::false_type { };
  template <typename T> struct _has_sumw2<T,
    void_t<decltype( std::declval<T>().sumw2(std::declval<Bin>()) )>
  > : std::true_type { };
  
  template <typename, typename = void>
  struct _has_num : std::false_type { };
  template <typename T> struct _has_num<T,
    void_t<decltype( std::declval<T>().num(std::declval<Bin>()) )>
  > : std::true_type { };

public:
  static constexpr bool has_weight = _has_weight<F>::value;
  static constexpr bool has_sumw2  = _has_sumw2 <F>::value;
  static constexpr bool has_num    = _has_num   <F>::value;
};

template <bool Use, typename Bins, typename F>
inline std::enable_if_t<!Use> set_weight(TH1* h, const Bins& bins, F get) { }
template <bool Use, typename Bins, typename F>
inline std::enable_if_t<Use> set_weight(TH1* h, const Bins& bins, F get) {
  Double_t *weight = dynamic_cast<TArrayD*>(h)->GetArray();
  size_t i = 0;
  for (const auto& bin : bins) weight[i] = get.weight(bin), ++i;
}

template <bool Use, typename Bins, typename F>
inline std::enable_if_t<!Use> set_sumw2(TH1* h, const Bins& bins, F get) { }
template <bool Use, typename Bins, typename F>
inline std::enable_if_t<Use> set_sumw2(TH1* h, const Bins& bins, F get) {
  h->Sumw2();
  Double_t *sumw2 = h->GetSumw2()->GetArray();
  size_t i = 0;
  for (const auto& bin : bins) sumw2[i] = get.sumw2(bin), ++i;
}

template <bool Use, typename Bins, typename F>
inline std::enable_if_t<!Use> set_num(TH1* h, const Bins& bins, F get) { }
template <bool Use, typename Bins, typename F>
inline std::enable_if_t<Use> set_num(TH1* h, const Bins& bins, F get) {
  Double_t n_total = 0;
  for (const auto& bin : bins) n_total += get.num(bin);
  h->SetEntries(n_total);
}

} // end namespace detail

template <typename Bin, typename... Ax, typename Container, typename Filler,
          typename F = detail::bin_get<Bin> >
auto* to_root(
  const binner<Bin,std::tuple<Ax...>,Container,Filler>& hist,
  const std::string& name,
  F get = F{}
) {
  auto *h = make_TH(name.c_str(),hist.axes());

  using bin_traits = detail::bin_get_traits<F,Bin>;

  detail::set_weight<bin_traits::has_weight>(h,hist.bins(),get);
  detail::set_sumw2 <bin_traits::has_sumw2 >(h,hist.bins(),get);
  detail::set_num   <bin_traits::has_num   >(h,hist.bins(),get);

  return h;
};

template <size_t D, typename Bin, typename... Ax, typename... Labels>
auto* to_root(
  const binner_slice<D, Bin, std::tuple<Ax...>>& hist,
  const std::string& name,
  Labels&&... labels
) {
  std::stringstream ss;
  ss.precision(3);
  ss << name << hist.name(std::forward<Labels>(labels)...);
  return to_root(*hist,ss.str());
};

} // end namespace root
} // end namespace ivanp

#endif
