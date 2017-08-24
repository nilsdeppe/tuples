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

static int global_of_no_default = 0;

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
