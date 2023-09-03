// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/time/instant.hh>

#include <nel/time/duration.hh>

#include <nel/memory.hh> // move

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace instant
{

using nel::move;
using nel::time::Instant;

using nel::time::Duration;

//============================================================================

TEST_CASE("Instant::move", "[instant]")
{
    // instants can be moved.
    {
        auto d1 = Instant();
        auto d2 = Instant(move(d1));

        NEL_UNUSED(d2);
    }

    {
        auto d1 = Instant();
        auto d2 = move(d1);

        NEL_UNUSED(d2);
    }
}

TEST_CASE("Instant::copy", "[instant]")
{
    // instants can be copied.
    {
        auto d1 = Instant();
        auto d2 = Instant(d1);
        NEL_UNUSED(d2);
    }
    {
        auto d1 = Instant();
        auto d2 = d1;
        NEL_UNUSED(d2);
    }
}

//============================================================================
// instants cannot be added together
// instants cannot be subtracted together

TEST_CASE("Instant::iadd", "[instant]")
{
    // durations can be added to instants, resulting in another instant.
    auto i1 = Instant();
    auto i2 = i1;
    auto d1 = Duration::from_micros(30);
    i2 += d1;

    REQUIRE(i2.elapsed_since(i1) == d1);
}

TEST_CASE("Instant::add", "[instant]")
{
    // duration can be added to another
    auto i1 = Instant();

    auto d1 = Duration::from_micros(30);

    auto i2 = i1 + d1;

    REQUIRE(i2.elapsed_since(i1) == d1);
}

TEST_CASE("Instant::isub", "[instant]")
{
    // instants can have durations subtracted from them
    {
        auto i1 = Instant();
        auto i2 = i1;
        auto d1 = Duration::from_micros(30);
        i2 += d1;

        i2 -= Duration::from_micros(10);

        REQUIRE(i2.elapsed_since(i1) == Duration::from_micros(30 - 10));
    }
    {
        // and can go negative. ?
        auto i1 = Instant();
        auto i2 = i1;
        auto d1 = Duration::from_micros(30);
        i2 += d1;

        i2 -= Duration::from_micros(40);

        REQUIRE(i2.elapsed_since(i1) == Duration::from_micros(30 - 40));
    }
}

TEST_CASE("Instant::sub", "[instant]")
{
    // instants can have durations subtracted from them
    {
        auto i1 = Instant();
        auto i2 = i1;
        auto d1 = Duration::from_micros(30);
        i2 += d1;

        auto i3 = i2 - Duration::from_micros(10);

        REQUIRE(i3.elapsed_since(i1) == Duration::from_micros(30 - 10));
    }
    {
        // and can go negative. ?
        auto i1 = Instant();
        auto i2 = i1;
        auto d1 = Duration::from_micros(30);
        i2 += d1;

        auto i3 = i2 - Duration::from_micros(40);

        REQUIRE(i3.elapsed_since(i1) == Duration::from_micros(30 - 40));
    }
}

//============================================================================
TEST_CASE("Instant::eq", "[instant]")
{
    // duration can be compared for equality
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(d1 == d2);
    }
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(30);
        REQUIRE(!(d1 == d2));
    }
}

TEST_CASE("Instant::ne", "[instant]")
{
    // duration can be compared for inequality
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(30);
        REQUIRE(d1 != d2);
    }
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(!(d1 != d2));
    }
}

TEST_CASE("Instant::lt", "[instant]")
{
    // duration can be compared for less than
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(30);
        REQUIRE(d1 < d2);
    }
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(!(d1 < d2));
    }
    {
        auto d1 = Instant() + Duration::from_millis(30);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(!(d1 < d2));
    }
}

TEST_CASE("Instant::gt", "[instant]")
{
    // duration can be compared for greater than
    {
        auto d1 = Instant() + Duration::from_millis(30);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(d1 > d2);
    }
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(!(d1 > d2));
    }
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(30);
        REQUIRE(!(d1 > d2));
    }
}

TEST_CASE("Instant::le", "[instant]")
{
    // duration can be compared for less than or equal to
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(30);
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = Instant() + Duration::from_millis(30);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(!(d1 <= d2));
    }
}

TEST_CASE("Instant::ge", "[instant]")
{
    // duration can be compared for greater than or equality
    {
        auto d1 = Instant() + Duration::from_millis(30);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(20);
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = Instant() + Duration::from_millis(20);
        auto d2 = Instant() + Duration::from_millis(30);
        REQUIRE(!(d1 >= d2));
    }
}

//============================================================================

TEST_CASE("Instant::default", "[instant]")
{
    // instants can be created without a value.
    auto d1 = Instant();
    NEL_UNUSED(d1);
}

// instants cannot be initialised with a value.

//=============================================================================

// the instant representation cannot be extracted.

}; // namespace instant
}; // namespace test
}; // namespace nel
