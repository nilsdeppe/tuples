/*!
@file
@copyright Nils Deppe 2017
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
*/

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

// TODO: remove later, used for debugging purposes
#include <iostream>
template <class...>
struct TD;

#include "tuples/tagged_tuple.hpp"

namespace {

static int global_of_no_default = 0;
static int global_time_mock = 0;

struct empty_base {};
bool operator==(const empty_base&, const empty_base&) noexcept { return true; }
struct no_default {
  no_default() = delete;
  explicit no_default(int i) { global_of_no_default = i; }
};
bool operator==(const no_default&, const no_default&) noexcept { return true; }

namespace tags {
struct empty_base {
  using type = ::empty_base;
};
struct no_default {
  using type = ::no_default;
};
struct no_default2 {
  using type = ::no_default;
};
}  // namespace tags

namespace tags {

struct Int {
  using type = int;
};
struct Int2 {
  using type = int;
};
}  // namespace tags

namespace {
template <class Tuple, class... Value>
Tuple test_impl(Value&&... value) {
  Tuple tuple(std::forward<Value>(value)...);
  return tuple;
}

template <class Tag, class Tuple, class... Values>
void check_get(Tuple& t, Values&&... values) {
  CHECK(tuples::get<Tag>(static_cast<Tuple&>(t)) ==
        typename Tag::type{values...});
  CHECK(tuples::get<Tag>(static_cast<const Tuple&>(t)) ==
        typename Tag::type{values...});
  CHECK(tuples::get<Tag>(static_cast<Tuple&&>(t)) ==
        typename Tag::type{values...});
  CHECK(tuples::get<Tag>(static_cast<const Tuple&&>(t)) ==
        typename Tag::type{values...});
}
}  // namespace

TEST_CASE("Unit.tagged_tuple.NoTag", "[Unit][Runtime][tagged_tuple]") {
  tuples::tagged_tuple<> dummy;
  tuples::tagged_tuple<> dummy1;
  dummy.swap(dummy1);
}

TEST_CASE("Unit.tagged_tuple.SingleTag", "[Unit][Runtime][tagged_tuple]") {
  const int i = 0;
  {
    auto t = test_impl<tuples::tagged_tuple<tags::Int>>(1);
    CHECK(tuples::get<tags::Int>(t) == 1);
  }
  {
    const auto t = test_impl<tuples::tagged_tuple<tags::Int>>(1);
    CHECK(tuples::get<tags::Int>(t) == 1);
  }

  {
    auto t = test_impl<tuples::tagged_tuple<tags::Int>>(1);
    CHECK(tuples::get<tags::Int>(std::move(t)) == 1);
  }
  {
    const auto t = test_impl<tuples::tagged_tuple<tags::Int>>(1);
    CHECK(tuples::get<tags::Int>(
              static_cast<const tuples::tagged_tuple<tags::Int>&&>(t)) == 1);
  }

  {
    auto t = test_impl<tuples::tagged_tuple<tags::Int>>(i);
    CHECK(tuples::get<tags::Int>(t) == i);
  }
  {
    const auto t = test_impl<tuples::tagged_tuple<tags::Int>>(i);
    CHECK(tuples::get<tags::Int>(t) == i);
  }

  {
    auto t = test_impl<tuples::tagged_tuple<tags::Int>>(i);
    CHECK(tuples::get<tags::Int>(std::move(t)) == i);
  }
  {
    const auto t = test_impl<tuples::tagged_tuple<tags::Int>>(i);
    CHECK(tuples::get<tags::Int>(
              static_cast<const tuples::tagged_tuple<tags::Int>&&>(t)) == i);
  }
}

TEST_CASE("Unit.tagged_tuple.Ebo", "[Unit][Runtime][tagged_tuple]") {
  const int i = 1;
  test_impl<tuples::tagged_tuple<tags::no_default>>(i);
  CHECK(global_of_no_default == 1);

  auto t =
      test_impl<tuples::tagged_tuple<tags::no_default, tags::no_default2,
                                     tags::empty_base>>(10, 100, empty_base{});
  CHECK((global_of_no_default == 10 or global_of_no_default == 100));
  check_get<tags::no_default>(t, 0);
  CHECK(global_of_no_default == 0);
  check_get<tags::no_default2>(t, 0);
  CHECK(global_of_no_default == 0);

  auto t2 = test_impl<tuples::tagged_tuple<tags::empty_base>>();
  check_get<tags::empty_base>(t2);
}

struct NotNoExceptCompare {
  explicit NotNoExceptCompare(int v) : v_(v) {}

  int v_{0};
};
bool operator==(NotNoExceptCompare const& lhs,
                NotNoExceptCompare const& rhs) noexcept(false) {
  return lhs.v_ == rhs.v_;
}
bool operator!=(NotNoExceptCompare const& lhs,
                NotNoExceptCompare const& rhs) noexcept(noexcept(lhs == rhs)) {
  return not(lhs == rhs);
}

struct timed_compare {
  explicit timed_compare(int v) : v_(v) {}
  int v_{0};
};
bool operator==(timed_compare const& lhs, timed_compare const& rhs) noexcept {
  global_time_mock++;
  return lhs.v_ == rhs.v_;
}

namespace relational_tags {
struct Int0 {
  using type = int;
};
struct Int1 {
  using type = int;
};
struct Int2 {
  using type = int;
};

struct NotNoExceptCompare {
  using type = ::NotNoExceptCompare;
};

struct TimedCompare0 {
  using type = ::timed_compare;
};
struct TimedCompare1 {
  using type = ::timed_compare;
};
struct TimedCompare2 {
  using type = ::timed_compare;
};
struct TimedCompare3 {
  using type = ::timed_compare;
};
struct TimedCompare4 {
  using type = ::timed_compare;
};
struct TimedCompare5 {
  using type = ::timed_compare;
};
struct TimedCompare6 {
  using type = ::timed_compare;
};
struct TimedCompare7 {
  using type = ::timed_compare;
};
struct TimedCompare8 {
  using type = ::timed_compare;
};
struct TimedCompare9 {
  using type = ::timed_compare;
};
struct TimedCompare10 {
  using type = ::timed_compare;
};
struct TimedCompare11 {
  using type = ::timed_compare;
};
struct TimedCompare12 {
  using type = ::timed_compare;
};
struct TimedCompare13 {
  using type = ::timed_compare;
};
}  // namespace relational_tags

TEST_CASE("Unit.tagged_tuple.relational", "[unit][runtime][tagged_tuple]") {
  {
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t0{1, 4, 5};
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t1{1, 4, 5};
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t2{1, 3, 5};
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t3{2, 4, 5};
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t4{1, 4, 6};
    CHECK(t0 == t1);
    CHECK(t0 == t0);
    CHECK_FALSE(t0 != t1);
    CHECK_FALSE(t0 == t2);
    CHECK(t0 != t2);
    CHECK_FALSE(t0 == t3);
    CHECK(t0 != t3);
    CHECK_FALSE(t0 == t4);
    CHECK(t0 != t4);
    // Check that short circuiting in comparisons works correctly
    tuples::tagged_tuple<
        relational_tags::TimedCompare0, relational_tags::TimedCompare1,
        relational_tags::TimedCompare2, relational_tags::TimedCompare3,
        relational_tags::TimedCompare4, relational_tags::TimedCompare5,
        relational_tags::TimedCompare6, relational_tags::TimedCompare7,
        relational_tags::TimedCompare8, relational_tags::TimedCompare9,
        relational_tags::TimedCompare10, relational_tags::TimedCompare11,
        relational_tags::TimedCompare12, relational_tags::TimedCompare13>
        t5{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    tuples::tagged_tuple<
        relational_tags::TimedCompare0, relational_tags::TimedCompare1,
        relational_tags::TimedCompare2, relational_tags::TimedCompare3,
        relational_tags::TimedCompare4, relational_tags::TimedCompare5,
        relational_tags::TimedCompare6, relational_tags::TimedCompare7,
        relational_tags::TimedCompare8, relational_tags::TimedCompare9,
        relational_tags::TimedCompare10, relational_tags::TimedCompare11,
        relational_tags::TimedCompare12, relational_tags::TimedCompare13>
        t6{1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    global_time_mock = 0;
    CHECK_FALSE(t5 == t6);
    CHECK(global_time_mock == 1);

    global_time_mock = 0;
    CHECK(t5 != t6);
    CHECK(global_time_mock == 1);

    tuples::tagged_tuple<
        relational_tags::TimedCompare0, relational_tags::TimedCompare1,
        relational_tags::TimedCompare2, relational_tags::TimedCompare3,
        relational_tags::TimedCompare4, relational_tags::TimedCompare5,
        relational_tags::TimedCompare6, relational_tags::TimedCompare7,
        relational_tags::TimedCompare8, relational_tags::TimedCompare9,
        relational_tags::TimedCompare10, relational_tags::TimedCompare11,
        relational_tags::TimedCompare12, relational_tags::TimedCompare13>
        t7{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14};
    global_time_mock = 0;
    CHECK_FALSE(t5 == t7);
    CHECK(global_time_mock == 14);

    global_time_mock = 0;
    CHECK(t5 != t7);
    CHECK(global_time_mock == 14);
  }
  static_assert(not noexcept(NotNoExceptCompare{1} == NotNoExceptCompare{1}),
                "Failed testing Unit.tagged_tuple.relational");
  static_assert(not noexcept(NotNoExceptCompare{1} != NotNoExceptCompare{0}),
                "Failed testing Unit.tagged_tuple.relational");
  CHECK(NotNoExceptCompare{1} == NotNoExceptCompare{1});
  CHECK_FALSE(NotNoExceptCompare{1} == NotNoExceptCompare{0});
  CHECK(NotNoExceptCompare{1} != NotNoExceptCompare{0});
  CHECK_FALSE(NotNoExceptCompare{1} != NotNoExceptCompare{1});

#if __cplusplus >= 201402L
  {
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t0{1, 4, 5};
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t1{1, 4, 5};
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t2{1, 3, 5};
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t3{2, 4, 5};
    constexpr tuples::tagged_tuple<relational_tags::Int0, relational_tags::Int1,
                                   relational_tags::Int2>
        t4{1, 4, 6};
    static_assert(t0 == t1, "Failed testing Unit.tagged_tuple.relational");
    static_assert(not(t0 == t2), "Failed testing Unit.tagged_tuple.relational");
    static_assert(not(t0 == t3), "Failed testing Unit.tagged_tuple.relational");
    static_assert(not(t0 == t4), "Failed testing Unit.tagged_tuple.relational");
    static_assert(t0 != t2, "Failed testing Unit.tagged_tuple.relational");
    static_assert(t0 != t3, "Failed testing Unit.tagged_tuple.relational");
    static_assert(t0 != t4, "Failed testing Unit.tagged_tuple.relational");
    static_assert(noexcept(t0 != t4),
                  "Failed testing Unit.tagged_tuple.relational");
    static_assert(noexcept(t0 == t2),
                  "Failed testing Unit.tagged_tuple.relational");
  }
#endif
}

TEST_CASE("Unit.tagged_tuple.helper_classes", "[unit][runtime][tagged_tuple]") {
  static_assert(
      tuples::tuple_size<
          tuples::tagged_tuple<tags::no_default, tags::empty_base>>::value == 2,
      "Failed check tuple_size");
  static_assert(tuples::tuple_size<const tuples::tagged_tuple<
                        tags::no_default, tags::empty_base>>::value == 2,
                "Failed check tuple_size");
  static_assert(tuples::tuple_size<volatile tuples::tagged_tuple<
                        tags::no_default, tags::empty_base>>::value == 2,
                "Failed check tuple_size");
  static_assert(tuples::tuple_size<const volatile tuples::tagged_tuple<
                        tags::no_default, tags::empty_base>>::value == 2,
                "Failed check tuple_size");

  static_assert(tuples::tuple_size<tuples::tagged_tuple<>>::value == 0,
                "Failed check tuple_size");
  static_assert(tuples::tuple_size<const tuples::tagged_tuple<>>::value == 0,
                "Failed check tuple_size");
  static_assert(tuples::tuple_size<volatile tuples::tagged_tuple<>>::value == 0,
                "Failed check tuple_size");
  static_assert(
      tuples::tuple_size<const volatile tuples::tagged_tuple<>>::value == 0,
      "Failed check tuple_size");
}
}  // namespace
