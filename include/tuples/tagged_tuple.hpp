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
namespace tuples_detail {

template <class T, T...>
struct value_list {};

template <bool... Bs>
struct all
    : std::is_same<value_list<bool, Bs...>,
                   value_list<bool, (static_cast<void>(Bs), true)...>>::type {};
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
      : value_(std::forward<T>(t)) {
    static_assert(can_bind_reference<T>(),
                  "Cannot construct an lvalue reference with an rvalue");
  }

  constexpr tagged_tuple_leaf(const tagged_tuple_leaf& /*rhs*/) = default;
  constexpr tagged_tuple_leaf(tagged_tuple_leaf&& /*rhs*/) = default;

#if __cplusplus <= 201402L
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
      : value_type(std::forward<T>(t)) {}

  constexpr tagged_tuple_leaf(const tagged_tuple_leaf& /*rhs*/) = default;
  constexpr tagged_tuple_leaf(tagged_tuple_leaf&& /*rhs*/) = default;

#if __cplusplus <= 201402L
  value_type& get() noexcept { return static_cast<value_type&>(*this); }
#else
  constexpr value_type& get() noexcept { return value_; }
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

template <class... Tags>
class tagged_tuple : private tuples_detail::tagged_tuple_leaf<Tags>... {
  template <class Tag>
  using tag_type = typename Tag::type;

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
                  std::is_same<tag_type<Ts>, Ts>::value...>::value);
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
                  std::is_same<tag_type<Ts>, Ts>::value...>::value);
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
  template <bool Dummy = true,
            typename std::enable_if<
                args_constructor<Dummy>::enable_default()>::type* = nullptr>
  constexpr tagged_tuple() noexcept(
      tuples_detail::all<std::is_nothrow_default_constructible<
          tag_type<Tags>>::value...>::value) {}

  constexpr tagged_tuple(const tagged_tuple& /*rhs*/) = default;
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
      : tuples_detail::tagged_tuple_leaf<Tags>(std::forward<Us>(us))... {}

  template <class... Us,
            typename std::enable_if<
                args_constructor<not pack_is_tagged_tuple<Us...>::value and
                                 sizeof...(Us) == sizeof...(Tags)>::
                    template enable_implicit<Us&&...>()>::type* = nullptr>
  constexpr tagged_tuple(Us&&... us) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tuples_detail::tagged_tuple_leaf<Tags>, Us&&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(std::forward<Us>(us))... {}

  template <class... Us,
            typename std::enable_if<tuple_like_constructor<
                sizeof...(Tags) == sizeof...(Us) and
                tuples_detail::all<std::is_constructible<tag_type<Tags>,
                                                         const Us&>::value...>::
                    value>::template enable_explicit<const tagged_tuple<Us...>&,
                                                     Us...>()>::type* = nullptr>
  constexpr explicit tagged_tuple(const tagged_tuple<Us...>& t) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tag_type<Tags>, const Us&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(get<Tags>(t))... {}

  template <class... Us,
            typename std::enable_if<tuple_like_constructor<
                sizeof...(Tags) == sizeof...(Us) and
                tuples_detail::all<std::is_constructible<tag_type<Tags>,
                                                         const Us&>::value...>::
                    value>::template enable_implicit<const tagged_tuple<Us...>&,
                                                     Us...>()>::type* = nullptr>
  constexpr tagged_tuple(const tagged_tuple<Us...>& t) noexcept(
      tuples_detail::all<std::is_nothrow_constructible<
          tag_type<Tags>, const Us&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(get<Tags>(t))... {}

  template <
      class... Us,
      typename std::enable_if<
          tuple_like_constructor<sizeof...(Tags) == sizeof...(Us) and
                                 tuples_detail::all<std::is_constructible<
                                     tag_type<Tags>, Us&&>::value...>::value>::
              template enable_explicit<tagged_tuple<Us...>&&, Us...>()>::type* =
          nullptr>
  constexpr explicit tagged_tuple(tagged_tuple<Us...>&& t) noexcept(
      tuples_detail::all<
          std::is_nothrow_constructible<tag_type<Tags>, Us&&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(
            std::forward<Us>(get<Tags>(t)))... {}

  template <
      class... Us,
      typename std::enable_if<
          tuple_like_constructor<sizeof...(Tags) == sizeof...(Us) and
                                 tuples_detail::all<std::is_constructible<
                                     tag_type<Tags>, Us&&>::value...>::value>::
              template enable_implicit<tagged_tuple<Us...>&&, Us...>()>::type* =
          nullptr>
  constexpr tagged_tuple(tagged_tuple<Us...>&& t) noexcept(
      tuples_detail::all<
          std::is_nothrow_constructible<tag_type<Tags>, Us&&>::value...>::value)
      : tuples_detail::tagged_tuple_leaf<Tags>(
            std::forward<Us>(get<Tags>(t)))... {}
};

template <>
class tagged_tuple<> {
 public:
  tagged_tuple() noexcept {}
  void swap(tagged_tuple& /*unused*/) noexcept {}
};

// get implementations
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
}  // namespace tuples
