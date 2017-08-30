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
// [[COMPILER: GNU REGEX: error: no matching function for call to
// ‘tuples::tagged_tuple<tags::Int>::tagged_tuple\(\)’]]
// [[COMPILER: Clang REGEX: error: no matching constructor for initialization of
// 'tuples::tagged_tuple<tags::Int>'.  tuples::tagged_tuple<tags::Int> dummy;]]
// [[COMPILER: AppleClang REGEX: error: no matching constructor for
// initialization of 'tuples::tagged_tuple<tags::Int>'.
//  tuples::tagged_tuple<tags::Int> dummy;]]

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

#ifdef COMPILATION_TEST_ASSIGNMENT_LVALUE_NST
// [[TAGS: unit, tagged_tuple, compilation_test]]
// [[COMPILER: GNU REGEX: error: no match for ‘operator=’ \(operand types are
// ‘tuples::tagged_tuple<tags::Int0, tags::Int1>’ and ‘int’\)]]
// [[COMPILER: Clang REGEX: error: no viable overloaded '='.*candidate function
// not viable: no known conversion from 'int' to 'const typename
// std::conditional<is_copy_assignable::value, tagged_tuple<Int0, Int1>,
// tuples_detail::no_such_type>::type']]
// [[COMPILER: AppleClang REGEX: error: no viable overloaded '='.*candidate
// function not viable: no known conversion from 'int' to 'const typename
// std::conditional<is_copy_assignable::value, tagged_tuple<Int0, Int1>,
// tuples_detail::no_such_type>::type']]

namespace tags {
struct Int0 {
  using type = int;
};
struct Int1 {
  using type = int;
};
}  // namespace tags

int main() {
  tuples::tagged_tuple<tags::Int0, tags::Int1> t0{1, 3};
  t0 = 8;
}
#endif

#ifdef COMPILATION_TEST_ASSIGNMENT_RVALUE_NST
// [[TAGS: unit, tagged_tuple, compilation_test]]
// [[COMPILER: GNU REGEX: error: no match for ‘operator=’ \(operand types are
// ‘tuples::tagged_tuple<tags::Int0, tags::Int1>’ and
// ‘std::remove_reference<tags::Int0>::type {aka tags::Int0}’\)]]
// [[COMPILER: Clang REGEX: candidate function not viable: no known
// conversion from 'typename remove_reference<Int0>::type' \(aka 'tags::Int0'\)
// to 'const typename std::conditional<is_copy_assignable::value,
// tagged_tuple<Int0,
// Int1>, tuples_detail::no_such_type>::type' \(aka 'const
// tuples::tagged_tuple<tags::Int0, tags::Int1>'\) for 1st argument]]
// [[COMPILER: AppleClang REGEX: candidate function not viable: no known
// conversion from 'typename remove_reference<Int0>::type' \(aka 'tags::Int0'\)
// to 'const typename std::conditional<is_copy_assignable::value,
// tagged_tuple<Int0,
// Int1>, tuples_detail::no_such_type>::type' \(aka 'const
// tuples::tagged_tuple<tags::Int0, tags::Int1>'\) for 1st argument]]

namespace tags {
struct Int0 {
  using type = int;
};
struct Int1 {
  using type = int;
};
}  // namespace tags

int main() {
  tuples::tagged_tuple<tags::Int0, tags::Int1> t0{1, 3};
  t0 = std::move(tags::Int0{});
}
#endif

#ifdef COMPILATION_TEST_ASSIGNMENT_LVALUE_TAGGED_TUPLE
// [[TAGS: unit, tagged_tuple, compilation_test]]

// [[COMPILER: GNU:0.0.0 REGEX: error: no type named ‘type’ in ‘struct
// std::enable_if<false, void>’.*tagged_tuple&
// operator=\(tagged_tuple<UTags...>.*co]]

// [[COMPILER: Clang:0.0.0 REGEX: error: no viable overloaded '='.*note:
// candidate template ignored: disabled by 'enable_if' \[with UTags =
// <tags::Int0, tags::my_class>]]

// [[COMPILER: Clang:5.0.0 REGEX: error: no viable overloaded '='.*note:
// candidate template ignored: requirement
// 'tuples_detail::all<std::is_assignable<tag_type<Int0> &, const tag_type<Int0>
// &>::value, std::is_assignable<tag_type<Int1> &, const tag_type<my_class>
// &>::value>::value' was not satisfied \[with UTags = <tags::Int0,
// tags::my_class>]]

// [[COMPILER: AppleClang:0.0.0 REGEX: error: no viable overloaded '='.*note:
// candidate template ignored: disabled by 'enable_if' \[with UTags =
// <tags::Int0, tags::my_class>]]

struct my_class {
  my_class(int) {}
};

namespace tags {
struct Int0 {
  using type = int;
};
struct Int1 {
  using type = int;
};
struct my_class {
  using type = ::my_class;
};
}  // namespace tags

int main() {
  tuples::tagged_tuple<tags::Int0, tags::Int1> t0{1, 3};
  tuples::tagged_tuple<tags::Int0, tags::my_class> t1{1, 3};
  t0 = t1;
}
#endif

#ifdef COMPILATION_TEST_ASSIGNMENT_RVALUE_TAGGED_TUPLE
// [[TAGS: unit, tagged_tuple, compilation_test]]

// [[COMPILER: GNU:0.0.0 REGEX: error: no type named ‘type’ in ‘struct
// std::enable_if<false, void>’.*tagged_tuple&
// operator=\(tagged_tuple<UTags...>&& t\)]]

// [[COMPILER: Clang:0.0.0 REGEX: error: no viable overloaded '='.*note:
// candidate template ignored: disabled by 'enable_if' \[with UTags =
// <tags::Int0, tags::my_class>]]

// [[COMPILER: Clang:5.0.0 REGEX: error: no viable overloaded '='.*note:
// candidate template ignored: requirement
// 'tuples_detail::all<std::is_assignable<tag_type<Int0> &, const tag_type<Int0>
// &>::value, std::is_assignable<tag_type<Int1> &, const tag_type<my_class>
// &>::value>::value' was not satisfied \[with UTags = <tags::Int0,
// tags::my_class>]]

// [[COMPILER: AppleClang:0.0.0 REGEX: error: no viable overloaded '='.*note:
// candidate template ignored: disabled by 'enable_if' \[with UTags =
// <tags::Int0, tags::my_class>]]

struct my_class {
  my_class(int) {}
};

namespace tags {
struct Int0 {
  using type = int;
};
struct Int1 {
  using type = int;
};
struct my_class {
  using type = ::my_class;
};
}  // namespace tags

int main() {
  tuples::tagged_tuple<tags::Int0, tags::Int1> t0{1, 3};
  tuples::tagged_tuple<tags::Int0, tags::my_class> t1{1, 3};
  t0 = std::move(t1);
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
