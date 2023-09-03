// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/time/timer.hh>

#include <nel/time/instant.hh>
#include <nel/time/duration.hh>

#include <nel/memory.hh> // move

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace timer
{

using nel::time::Timer;

using nel::move;
using nel::time::Duration;
using nel::time::Instant;

//============================================================================

TEST_CASE("Timer::move", "[timer]")
{
    // Timers can be moved.
    {
        auto d1 = Timer(Duration::from_millis(10), Instant());
        auto d2 = Timer(move(d1));

        NEL_UNUSED(d2);
    }

    {
        auto d1 = Timer(Duration::from_millis(10), Instant());
        auto d2 = move(d1);

        NEL_UNUSED(d2);
    }
}

TEST_CASE("Timer::copy", "[timer]")
{
    // Timers can be copied.
    {
        auto d1 = Timer(Duration::from_millis(10), Instant());
        auto d2 = Timer(d1);
        NEL_UNUSED(d2);
    }
    {
        auto d1 = Timer(Duration::from_millis(10), Instant());
        auto d2 = d1;
        NEL_UNUSED(d2);
    }
}

//============================================================================

TEST_CASE("Timer::has_expired", "[timer]")
{
    auto start = Instant();
    auto t1 = Timer(Duration::from_millis(10), start);

    // at t = 0ms, t=-10ms, timer must not expire;
    REQUIRE(!t1.has_expired(start));

    // at t = 5ms, t=-5ms, timer must not expire;
    REQUIRE(!t1.has_expired(start + Duration::from_millis(5)));

    // at t = 9ms, t=-1ms, timer must not expire;
    REQUIRE(!t1.has_expired(start + Duration::from_millis(9)));

    // at t = 10ms, t=0, timer must expire;
    REQUIRE(t1.has_expired(start + Duration::from_millis(10)));

    // at t = 11ms, t=+1ms, timer must expire;
    REQUIRE(t1.has_expired(start + Duration::from_millis(11)));
}

} // namespace timer
} // namespace test
} // namespace nel
