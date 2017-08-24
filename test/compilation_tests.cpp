/*!
@file
@copyright Nils Deppe 2017
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
*/

#include "tuples/tagged_tuple.hpp"

struct no_default {
  no_default() = delete;
  no_default(int) {}
};

namespace tags {
struct no_default {
  using type = ::no_default;
};
}  // namespace tags

#ifdef COMPILATION_TEST_DEFAULT_LVALUE
// [[TAGS: tag1, Tag3]]
// [[COMPILER: all REGEX: error: no matching function for call to
// ‘tuples::tagged_tuple<tags::Int>::tagged_tuple\(\)’]]

namespace tags {
struct Int {
  using type = int&;
};
}  // namespace tags

int main() {
  const int i = 0;
  tuples::tagged_tuple<tags::Int> dummy;
}
#endif

FILE_IS_COMPILATION_TEST

// template <class Dummy, bool IsSizeOne = sizeof...(Tags) == 1>
// struct TupleConstructorCheck {

//   // The Ts are already `const Ts&` or `Ts&&`
//   template <class... Ts>
//   static constexpr bool enable_implicit_constructor() noexcept {
//     return sizeof...(Ts) == sizeof...(Tags) and
//            detail::all<
//                std::is_constructible<tag_type<Tags>, Ts>::value...>::value
//                and
//            detail::all<
//                std::is_convertible<Ts, tag_type<Tags>>::value...>::value;
//   }
//   template <class... Ts>
//   static constexpr bool enable_explicit_constructor() noexcept {
//     return sizeof...(Ts) == sizeof...(Tags) and
//            detail::all<
//                std::is_constructible<tag_type<Tags>, Ts>::value...>::value
//                and
//            not detail::all<
//                std::is_convertible<Ts, tag_type<Tags>>::value...>::value;
//   }
// };

// template <class Dummy>
// struct TupleConstructorCheck<Dummy, false> {
//   // The Ts are already `const Ts&` or `Ts&&`
//   template <class... UTypes>
//   static constexpr bool enable_implicit_constructor() noexcept {
//     return sizeof...(UTypes) == sizeof...(Tags) and
//            detail::all<std::is_constructible<tag_type<Tags>,
//                                              UTypes>::value...>::value and
//            detail::all<
//                std::is_convertible<UTypes, tag_type<Tags>>::value...>::value
//                and
//            (not);
//   }
// };

// template <class TaggedTuple, typename std::enable_if<>::type* = nullptr>
// constexpr tagged_tuple(TaggedTuple&& t) noexcept()
//     : detail::tagged_tuple_leaf<Tags>(
//           std::forward<decltype(get<Tags>(t))>(get<Tags>(t)))... {}
