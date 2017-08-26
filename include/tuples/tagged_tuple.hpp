/*!
@file
@copyright Nils Deppe 2017
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
*/

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

namespace tuples {

#if __cplusplus >= 201402L
#define TUPLES_LIB_CONSTEXPR_CXX_14 constexpr
#else
#define TUPLES_LIB_CONSTEXPR_CXX_14
#endif

namespace tuples_detail {

template <class T>
inline constexpr T&& forward(
    typename std::remove_reference<T>::type& t) noexcept {
  return static_cast<T&&>(t);
}

template <class T>
inline constexpr T&& forward(
    typename std::remove_reference<T>::type&& t) noexcept {
  static_assert(!std::is_lvalue_reference<T>::value,
                "cannot forward an rvalue as an lvalue");
  return static_cast<T&&>(t);
}

template <class T, T...>
struct value_list {};

template <class...>
struct typelist {};

template <bool... Bs>
using all = typename std::is_same<
    value_list<bool, Bs...>,
    value_list<bool, (static_cast<void>(Bs), true)...>>::type;

struct no_such_type {
  no_such_type() = delete;
  no_such_type(no_such_type const& /*unused*/) = delete;
  no_such_type(no_such_type&& /*unused*/) = delete;
  ~no_such_type() = delete;
  no_such_type& operator=(no_such_type const& /*unused*/) = delete;
  no_such_type operator=(no_such_type&& /*unused*/) = delete;
};

template <typename... Ts>
constexpr char swallow(Ts&&...) noexcept {
  return '0';
}
}  // namespace tuples_detail

namespace tuples_detail {
template <class Tag,
          bool Ebo = std::is_empty<typename Tag::type>::value &&
                     !__is_final(typename Tag::type)>
class tagged_tuple_leaf {
  using value_type = typename Tag::type;
  value_type value_;

  template <class T>
  static constexpr bool can_bind_reference() noexcept {
    using rem_ref_value_type = typename std::remove_reference<value_type>::type;
    using rem_ref_T = typename std::remove_reference<T>::type;
    using is_lvalue_type = std::integral_constant<
        bool,
        std::is_lvalue_reference<T>::value or
            std::is_same<std::reference_wrapper<rem_ref_value_type>,
                         rem_ref_T>::value or
            std::is_same<std::reference_wrapper<typename std::remove_const<
                             rem_ref_value_type>::type>,
                         rem_ref_T>::value>;
    return not std::is_reference<value_type>::value or
           (std::is_lvalue_reference<value_type>::value and
            is_lvalue_type::value) or
           (std::is_rvalue_reference<value_type>::value and
            not std::is_lvalue_reference<T>::value);
  }

 public:
  // Tested in constexpr context in Unit.tagged_tuple.Ebo
  constexpr tagged_tuple_leaf() noexcept(
      std::is_nothrow_default_constructible<value_type>::value)
      : value_() {
    static_assert(
        !std::is_reference<value_type>::value,
        "Cannot default construct a reference element in a tagged_tuple");
  }

  template <class T,
            typename std::enable_if<
                !std::is_same<typename std::decay<T>::type,
                              tagged_tuple_leaf>::value &&
                std::is_constructible<value_type, T&&>::value>::type* = nullptr>
  constexpr explicit tagged_tuple_leaf(T&& t) noexcept(
      std::is_nothrow_constructible<value_type, T&&>::value)
      : value_(tuples_detail::forward<T>(t)) {
    static_assert(can_bind_reference<T>(),
                  "Cannot construct an lvalue reference with an rvalue");
  }

  constexpr tagged_tuple_leaf(tagged_tuple_leaf const& /*rhs*/) = default;
  constexpr tagged_tuple_leaf(tagged_tuple_leaf&& /*rhs*/) = default;

#if __cplusplus < 201402L
  value_type& get() noexcept { return value_; }
#else
  constexpr value_type& get() noexcept { return value_; }
#endif
  constexpr const value_type& get() const noexcept { return value_; }
};

template <class Tag>
class tagged_tuple_leaf<Tag, true> : private Tag::type {
  using value_type = typename Tag::type;

 public:
  constexpr tagged_tuple_leaf() noexcept(
      std::is_nothrow_default_constructible<value_type>::value)
      : value_type{} {}

  template <class T,
            typename std::enable_if<
                !std::is_same<typename std::decay<T>::type,
                              tagged_tuple_leaf>::value &&
                std::is_constructible<value_type, T&&>::value>::type* = nullptr>
  constexpr explicit tagged_tuple_leaf(T&& t) noexcept(
      std::is_nothrow_constructible<value_type, T&&>::value)
      : value_type(tuples_detail::forward<T>(t)) {}

  constexpr tagged_tuple_leaf(tagged_tuple_leaf const& /*rhs*/) = default;
  constexpr tagged_tuple_leaf(tagged_tuple_leaf&& /*rhs*/) = default;

#if __cplusplus < 201402L
  value_type& get() noexcept { return static_cast<value_type&>(*this); }
#else
  constexpr value_type& get() noexcept {
    return static_cast<value_type&>(*this);
  }
#endif

  constexpr const value_type& get() const noexcept {
    return static_cast<const value_type&>(*this);
  }
};

struct disable_constructors {
  static constexpr bool enable_default() noexcept { return false; }
  static constexpr bool enable_explicit() noexcept { return false; }
  static constexpr bool enable_implicit() noexcept { return false; }
};
}  // namespace tuples_detail

template <class... Tags>
class tagged_tuple;

template <class Tag, class... Tags>
constexpr const typename Tag::type& get(
    const tagged_tuple<Tags...>& t) noexcept;
template <class Tag, class... Tags>
constexpr typename Tag::type& get(tagged_tuple<Tags...>& t) noexcept;
template <class Tag, class... Tags>
constexpr const typename Tag::type&& get(
    const tagged_tuple<Tags...>&& t) noexcept;
template <class Tag, class... Tags>
constexpr typename Tag::type&& get(tagged_tuple<Tags...>&& t) noexcept;

/*!
 * \brief Returns the type of the Tag
 */
template <class Tag>
using tag_type = typename Tag::type;

template <class... Tags>
class tagged_tuple : private tuples_detail::tagged_tuple_leaf<Tags>... {
  template <class... Args>
  struct pack_is_tagged_tuple : std::false_type {};
  template <class Arg>
  struct pack_is_tagged_tuple<Arg>
      : std::is_same<typename std::decay<Arg>::type, tagged_tuple> {};

  template <bool EnableConstructor, class Dummy = void>
  struct args_constructor : tuples_detail::disable_constructors {};

  template <class Dummy>
  struct args_constructor<true, Dummy> {
    static constexpr bool enable_default() {
      return tuples_detail::all<
          std::is_default_constructible<tag_type<Tags>>::value...>::value;
    }

    template <class... Ts>
    static constexpr bool enable_explicit() noexcept {
      return tuples_detail::all<
                 std::is_constructible<tuples_detail::tagged_tuple_leaf<Tags>,
                                       Ts>::value...>::value and
             not tuples_detail::all<
                 std::is_convertible<Ts, tag_type<Tags>>::value...>::value;
    }
    template <class... Ts>
    static constexpr bool enable_implicit() noexcept {
      return sizeof...(Ts) == sizeof...(Tags) and
             tuples_detail::all<
                 std::is_constructible<tuples_detail::tagged_tuple_leaf<Tags>,
                                       Ts>::value...>::value and
             tuples_detail::all<
                 std::is_convertible<Ts, tag_type<Tags>>::value...>::value;
    }
  };

  template <bool EnableConstructor, bool = sizeof...(Tags) == 1,
            class Dummy = void>
  struct tuple_like_constructor : tuples_detail::disable_constructors {};

  template <class Dummy>
  struct tuple_like_constructor<true, false, Dummy> {
    template <class Tuple, class... Ts>
    static constexpr bool enable_explicit() noexcept {
      return not tuples_detail::all<
          std::is_convertible<Ts, tag_type<Tags>>::value...>::value;
    }

    template <class Tuple, class... Ts>
    static constexpr bool enable_implicit() noexcept {
      return tuples_detail::all<
          std::is_convertible<Ts, tag_type<Tags>>::value...>::value;
    }
  };

  template <class Dummy>
  struct tuple_like_constructor<true, true, Dummy> {
    template <class Tuple, class... Ts>
    static constexpr bool enable_explicit() noexcept {
      return not tuples_detail::all<
                 std::is_convertible<Ts, tag_type<Tags>>::value...>::value and
             (not tuples_detail::all<std::is_convertible<
                  Tuple, tag_type<Tags>>::value...>::value and
              not tuples_detail::all<std::is_constructible<
                  tag_type<Tags>, Tuple>::value...>::value and
              not tuples_detail::all<
                  std::is_same<tag_type<Tags>, Ts>::value...>::value);
    }

    template <class Tuple, class... Ts>
    static constexpr bool enable_implicit() noexcept {
      return tuples_detail::all<
                 std::is_convertible<Ts, tag_type<Tags>>::value...>::value and
             (not tuples_detail::all<std::is_convertible<
                  Tuple, tag_type<Tags>>::value...>::value and
              not tuples_detail::all<std::is_constructible<
                  tag_type<Tags>, Tuple>::value...>::value and
              not tuples_detail::all<
                  std::is_same<tag_type<Tags>, Ts>::value...>::value);
    }
  };

  template <class Tag, class... LTags>
  friend constexpr const typename Tag::type& get(
      const tagged_tuple<LTags...>& t) noexcept;
  template <class Tag, class... LTags>
  friend constexpr typename Tag::type& get(tagged_tuple<LTags...>& t) noexcept;
  template <class Tag, class... LTags>
  friend constexpr const typename Tag::type&& get(
      const tagged_tuple<LTags...>&& t) noexcept;
  template <class Tag, class... LTags>
  friend constexpr typename Tag::type&& get(
      tagged_tuple<LTags...>&& t) noexcept;

 public:
  // C++17 Draft 23.5.3.1 Construction
  template <bool Dummy = true,
            typename std::enable_if<
                args_constructor<Dummy>::enable_default()>::type* = nullptr>
  constexpr tagged_tuple() noexcept(
      tuples_detail::all<std::is_nothrow_default_constructible<
          tag_type<Tags>>::value...>::value) {}

  constexpr tagged_tuple(tagged_tuple const& /*rhs*/) = default;
  constexpr tagged_tuple(tagged_tuple&& /*rhs*/) = default;

  template <
      bool Dummy = true,
      typename std::enable_if<args_constructor<Dummy>::template enable_explicit<
          tag_type<Tags> const&...>()>::type* = nullptr>
  constexpr explicit tagged_tuple(tag_type<Tags> const&... ts) noexcept(
      tuples_detail::all<std::is_nothrow_copy_constructible<
          tuples_detail::tagged_tuple_leaf<Tags>>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(ts)... {}

  template <
      bool Dummy = true,
      typename std::enable_if<args_constructor<Dummy>::template enable_implicit<
          tag_type<Tags> const&...>()>::type* = nullptr>
  constexpr tagged_tuple(tag_type<Tags> const&... ts) noexcept(
      tuples_detail::all<std::is_nothrow_copy_constructible<
          tuples_detail::tagged_tuple_leaf<Tags>>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(ts)... {}

  template <class... Us,
            typename std::enable_if<
                args_constructor<not pack_is_tagged_tuple<Us...>::value and
                                 sizeof...(Us) == sizeof...(Tags)>::
                    template enable_explicit<Us&&...>()>::type* = nullptr>
  constexpr explicit tagged_tuple(Us&&... us) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tuples_detail::tagged_tuple_leaf<Tags>, Us&&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(
            tuples_detail::forward<Us>(us))... {}

  template <class... Us,
            typename std::enable_if<
                args_constructor<not pack_is_tagged_tuple<Us...>::value and
                                 sizeof...(Us) == sizeof...(Tags)>::
                    template enable_implicit<Us&&...>()>::type* = nullptr>
  constexpr tagged_tuple(Us&&... us) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tuples_detail::tagged_tuple_leaf<Tags>, Us&&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(
            tuples_detail::forward<Us>(us))... {}

  template <
      class... UTags,
      typename std::enable_if<
          tuple_like_constructor<
              sizeof...(Tags) == sizeof...(UTags) and
              tuples_detail::all<std::is_constructible<
                  tag_type<Tags>, tag_type<UTags> const&>::value...>::value>::
              template enable_explicit<tagged_tuple<UTags...> const&,
                                       tag_type<UTags>...>()>::type* = nullptr>
  constexpr explicit tagged_tuple(tagged_tuple<UTags...> const& t) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tag_type<Tags>, tag_type<UTags> const&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(get<Tags>(t))... {}

  template <
      class... UTags,
      typename std::enable_if<
          tuple_like_constructor<
              sizeof...(Tags) == sizeof...(UTags) and
              tuples_detail::all<std::is_constructible<
                  tag_type<Tags>, tag_type<UTags> const&>::value...>::value>::
              template enable_implicit<tagged_tuple<UTags...> const&,
                                       tag_type<UTags>...>()>::type* = nullptr>
  constexpr tagged_tuple(tagged_tuple<UTags...> const& t) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tag_type<Tags>, tag_type<UTags> const&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(get<Tags>(t))... {}

  template <
      class... UTags,
      typename std::enable_if<
          tuple_like_constructor<
              sizeof...(Tags) == sizeof...(UTags) and
              tuples_detail::all<std::is_constructible<
                  tag_type<Tags>, tag_type<UTags>&&>::value...>::value>::
              template enable_explicit<tagged_tuple<UTags...>&&,
                                       tag_type<UTags>...>()>::type* = nullptr>
  constexpr explicit tagged_tuple(tagged_tuple<UTags...>&& t) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tag_type<Tags>, tag_type<UTags>&&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(
            tuples_detail::forward<tag_type<UTags>>(get<Tags>(t)))... {}

  template <
      class... UTags,
      typename std::enable_if<
          tuple_like_constructor<
              sizeof...(Tags) == sizeof...(UTags) and
              tuples_detail::all<std::is_constructible<
                  tag_type<Tags>, tag_type<UTags>&&>::value...>::value>::
              template enable_implicit<tagged_tuple<UTags...>&&,
                                       tag_type<UTags>...>()>::type* = nullptr>
  constexpr tagged_tuple(tagged_tuple<UTags...>&& t) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tag_type<Tags>, tag_type<UTags>&&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(
            tuples_detail::forward<tag_type<UTags>>(get<Tags>(t)))... {}
};

template <>
class tagged_tuple<> {
 public:
  tagged_tuple() noexcept {}
  void swap(tagged_tuple& /*unused*/) noexcept {}
};

// C++17 Draft 23.5.3.6 Tuple helper classes
template <class T>
struct tuple_size;

template <class... Tags>
struct tuple_size<tagged_tuple<Tags...>>
    : std::integral_constant<size_t, sizeof...(Tags)> {};
template <class... Tags>
struct tuple_size<const tagged_tuple<Tags...>>
    : tuple_size<tagged_tuple<Tags...>> {};
template <class... Tags>
struct tuple_size<volatile tagged_tuple<Tags...>>
    : tuple_size<tagged_tuple<Tags...>> {};
template <class... Tags>
struct tuple_size<const volatile tagged_tuple<Tags...>>
    : tuple_size<tagged_tuple<Tags...>> {};

// C++17 Draft 23.5.3.7 Element access
template <class Tag, class... Tags>
inline constexpr const typename Tag::type& get(
    const tagged_tuple<Tags...>& t) noexcept {
  return static_cast<const tuples_detail::tagged_tuple_leaf<Tag>&>(t).get();
}
template <class Tag, class... Tags>
inline constexpr typename Tag::type& get(tagged_tuple<Tags...>& t) noexcept {
  return static_cast<tuples_detail::tagged_tuple_leaf<Tag>&>(t).get();
}
template <class Tag, class... Tags>
inline constexpr const typename Tag::type&& get(
    const tagged_tuple<Tags...>&& t) noexcept {
  return static_cast<const typename Tag::type&&>(
      static_cast<const tuples_detail::tagged_tuple_leaf<Tag>&&>(t).get());
}
template <class Tag, class... Tags>
inline constexpr typename Tag::type&& get(tagged_tuple<Tags...>&& t) noexcept {
  return static_cast<typename Tag::type&&>(
      static_cast<tuples_detail::tagged_tuple_leaf<Tag>&&>(t).get());
}

// C++17 Draft 23.5.3.8 Relational operators
namespace tuples_detail {
struct equal {
  template <class T, class U>
  static TUPLES_LIB_CONSTEXPR_CXX_14 void apply(
      T const& lhs, U const& rhs, bool& result) noexcept(noexcept(lhs == rhs)) {
    result = result and lhs == rhs;
  }
};

template <class... LTags, class... RTags>
TUPLES_LIB_CONSTEXPR_CXX_14 bool tuple_equal_impl(
    tagged_tuple<LTags...> const& lhs,
    tagged_tuple<RTags...> const&
        rhs) noexcept(noexcept(std::initializer_list<bool>{
    (get<LTags>(lhs) == get<RTags>(rhs))...})) {
  bool equal = true;
  // This short circuits in the sense that the operator== is only evaluated if
  // the result thus far is true
  static_cast<void>(std::initializer_list<char>{
      (equal::apply(get<LTags>(lhs), get<RTags>(rhs), equal), '0')...});
  return equal;
}
}  // namespace tuples_detail

template <class... LTags, class... RTags,
          typename std::enable_if<sizeof...(LTags) == sizeof...(RTags)>::type* =
              nullptr>
TUPLES_LIB_CONSTEXPR_CXX_14 bool operator==(
    tagged_tuple<LTags...> const& lhs,
    tagged_tuple<RTags...> const&
        rhs) noexcept(noexcept(tuples_detail::tuple_equal_impl(lhs, rhs))) {
  return tuples_detail::tuple_equal_impl(lhs, rhs);
}

template <class... LTags, class... RTags,
          typename std::enable_if<sizeof...(LTags) == sizeof...(RTags)>::type* =
              nullptr>
TUPLES_LIB_CONSTEXPR_CXX_14 bool operator!=(
    tagged_tuple<LTags...> const& lhs,
    tagged_tuple<RTags...> const& rhs) noexcept(noexcept(lhs == rhs)) {
  return not(lhs == rhs);
}
}  // namespace tuples
