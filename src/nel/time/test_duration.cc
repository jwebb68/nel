// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "duration.hh"

#include <nel/memory.hh> // move
#include <nel/defs.hh> // unused
#include <nel/types.hh> // ""_u64

#include <catch2/catch.hpp>
#include <catch2/catch2_panic.hh>

namespace nel
{
namespace test
{
namespace duration
{

using nel::time::Duration;
using nel::time::Frequency;

#if 0
//============================================================================
TEST_CASE("Timespec::eq", "[timespec]")
{
    {
        auto t1 = Timespec {123, 234};
        auto t2 = Timespec {123, 234};
        REQUIRE(t1 == t2);
    }
    {
        auto t1 = Timespec {123, 234};
        auto t2 = Timespec {123, 235};
        REQUIRE(!(t1 == t2));
    }
    {
        auto t1 = Timespec {123, 234};
        auto t2 = Timespec {124, 234};
        REQUIRE(!(t1 == t2));
    }
    {
        auto t1 = Timespec {123, 234};
        auto t2 = Timespec {124, 235};
        REQUIRE(!(t1 == t2));
    }
}

TEST_CASE("Timespec::ne", "[timespec]")
{
    {
        auto t1 = Timespec {123, 234};
        auto t2 = Timespec {123, 234};
        REQUIRE(!(t1 != t2));
    }
    {
        auto t1 = Timespec {123, 234};
        auto t2 = Timespec {123, 235};
        REQUIRE(t1 != t2);
    }
    {
        auto t1 = Timespec {123, 234};
        auto t2 = Timespec {124, 234};
        REQUIRE(t1 != t2);
    }
    {
        auto t1 = Timespec {123, 234};
        auto t2 = Timespec {124, 235};
        REQUIRE(t1 != t2);
    }
}

#endif
//============================================================================

TEST_CASE("Duration::move", "[duration]")
{
    // durations can be moved.
    {
        auto d1 = Duration();
        auto d2 = Duration(move(d1));

        nel::unused(d2);
    }

    {
        auto d1 = Duration();
        auto d2 = move(d1);

        nel::unused(d2);
    }
}

TEST_CASE("Duration::copy", "[duration]")
{
    // durations can be copied.
    {
        auto d1 = Duration();
        auto d2 = Duration(d1);
        nel::unused(d2);
    }
    {
        auto d1 = Duration();
        auto d2 = d1;
        nel::unused(d2);
    }
}

//============================================================================
TEST_CASE("Duration::iadd", "[duration]")
{
    // duration can be added to another
    auto d1 = 20_us;
    auto d2 = 30_us;
    d1 += d2;
    REQUIRE(d1 == 50_us);
}

TEST_CASE("Duration::add", "[duration]")
{
    // duration can be added to another
    auto d1 = 20_us;
    auto d2 = 30_us;
    auto d3 = d1 + d2;
    REQUIRE(d3 == 50_us);
}

TEST_CASE("Duration::isub", "[duration]")
{
    // duration can be subtracted from another
    {
        auto d1 = 20_us;
        auto d2 = 10_us;
        d1 -= d2;
        REQUIRE(d1 == 10_us);
    }

    // {
    //     // and can go negative. ?
    //     //not yet..
    //     auto d1 = 20_us;
    //     auto d2 = 50_us;
    //     d1 -= d2;
    //     REQUIRE(d1 == -30_us);
    // }
}

TEST_CASE("Duration::sub", "[duration]")
{
    // duration can be subtracted from another
    {
        auto d1 = 20_us;
        auto d2 = 10_us;
        auto d3 = d1 - d2;
        REQUIRE(d3 == 10_us);
    }
    // {
    //     // and can go negative. ? not yet.
    //     auto d1 = 20_us;
    //     auto d2 = 50_us;
    //     auto d3 = d1 - d2;
    //     REQUIRE(d3 == -30_us);
    // }
}

TEST_CASE("Duration::imul(uint64)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 20_us;
        d1 *= 1234_u64;
        REQUIRE(d1 == 24680_us);
    }
}

TEST_CASE("Duration::imul(float)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 20_us;
        d1 *= 1234.0f;
        REQUIRE(d1 == 24680.0_us);
    }
}

TEST_CASE("Duration::imul(double)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 20_us;
        d1 *= 1234.0;
        REQUIRE(d1 == 24680.0_us);
    }
}

TEST_CASE("Duration::mul(uint64)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 20_us;
        auto d2 = d1 * 1234_u64;
        REQUIRE(d2 == 24680.0_us);
    }
}

TEST_CASE("Duration::mul(float)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 20_us;
        auto d2 = d1 * 1234.0f;
        REQUIRE(d2 == 24680.0_us);
    }
}

TEST_CASE("Duration::mul(double)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 20_us;
        auto d2 = d1 * 1234.0;
        REQUIRE(d2 == 24680.0_us);
    }
}

TEST_CASE("Duration::idiv(uint64)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 2468_us;
        d1 /= 1234_u64;
        REQUIRE(d1 == 2.0_us);
        // auto v1 = d1.as_micros();
        // auto v2 = 20345UL/1234UL;
        // REQUIRE(v1 == v2);
    }
}

TEST_CASE("Duration::idiv(float)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 2468_us;
        d1 /= 1234.0f;
        REQUIRE(d1 == 2.0_us);
    }
}

TEST_CASE("Duration::idiv(double)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 2468_us;
        d1 /= 1234.0;
        REQUIRE(d1 == 2.0_us);
    }
}

TEST_CASE("Duration::div(uint64)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 2468_us;
        auto d2 = d1 / 1234_u64;
        REQUIRE(d2 == 2.0_us);
        // auto v1 = d1.as_micros();
        // auto v2 = 20345UL/1234UL;
        // REQUIRE(v1 == v2);
    }
}

TEST_CASE("Duration::div(float)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 2468_us;
        auto d2 = d1 / 1234.0f;
        REQUIRE(d2 == 2.0_us);
    }
}

TEST_CASE("Duration::div(double)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = 2468_us;
        auto d2 = d1 / 1234.0;
        REQUIRE(d2 == 2.0_us);
    }
}

//============================================================================
TEST_CASE("Duration::eq", "[duration]")
{
    // duration can be compared for equality
    {
        auto d1 = 20_ms;
        auto d2 = 20_ms;
        REQUIRE(d1 == d2);
    }
    {
        auto d1 = 20_ms;
        auto d2 = 30_ms;
        REQUIRE_FALSE(d1 == d2);
    }
}

TEST_CASE("Duration::ne", "[duration]")
{
    // duration can be compared for inequality
    {
        auto d1 = 20_ms;
        auto d2 = 30_ms;
        REQUIRE(d1 != d2);
    }
    {
        auto d1 = 20_ms;
        auto d2 = 20_ms;
        REQUIRE_FALSE(d1 != d2);
    }
}

TEST_CASE("Duration::lt", "[duration]")
{
    // duration can be compared for less than
    {
        auto d1 = 20_ms;
        auto d2 = 30_ms;
        REQUIRE(d1 < d2);
    }
    {
        auto d1 = 20_ms;
        auto d2 = 20_ms;
        REQUIRE_FALSE(d1 < d2);
    }
    {
        auto d1 = 30_ms;
        auto d2 = 20_ms;
        REQUIRE_FALSE(d1 < d2);
    }
}

TEST_CASE("Duration::gt", "[duration]")
{
    // duration can be compared for greater than
    {
        auto d1 = 30_ms;
        auto d2 = 20_ms;
        REQUIRE(d1 > d2);
    }
    {
        auto d1 = 20_ms;
        auto d2 = 20_ms;
        REQUIRE_FALSE(d1 > d2);
    }
    {
        auto d1 = 20_ms;
        auto d2 = 30_ms;
        REQUIRE_FALSE(d1 > d2);
    }
}

TEST_CASE("Duration::le", "[duration]")
{
    // duration can be compared for less than or equal to
    {
        auto d1 = 20_ms;
        auto d2 = 30_ms;
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = 20_ms;
        auto d2 = 20_ms;
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = 30_ms;
        auto d2 = 20_ms;
        REQUIRE_FALSE(d1 <= d2);
    }
}

TEST_CASE("Duration::ge", "[duration]")
{
    // duration can be compared for greater than or equality
    {
        auto d1 = 30_ms;
        auto d2 = 20_ms;
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = 20_ms;
        auto d2 = 20_ms;
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = 20_ms;
        auto d2 = 30_ms;
        REQUIRE_FALSE(d1 >= d2);
    }
}

//============================================================================

TEST_CASE("Duration::default", "[duration]")
{
    // durations can be created without a value.
    auto d1 = Duration();
    nel::unused(d1);
}

TEST_CASE("Duration::from_micros", "[duration]")
{
    // durations can be created from a  micros value
    auto d1 = 1234_us;
    nel::unused(d1);
}

TEST_CASE("from millis", "[duration]")
{
    // durations can be created from a millis value
    auto d1 = 1234_ms;
    nel::unused(d1);
}

TEST_CASE("Duration::from_secs", "[duration]")
{
    // durations can be created from a secs value
    auto d1 = 1234_s;
    nel::unused(d1);
}

TEST_CASE("Duration::from(secs, usecs)", "[duration]")
{
    // durations can be created from a secs+usecs value
    auto d1 = Duration::from(1234, 5678);
    nel::unused(d1);
}

#if 0
TEST_CASE("Duration::from(Timespec)", "[duration]")
{
    // durations can be created from a Timespec
    auto d1 = Duration::from(Timespec {1234, 5456});
    nel::unused(d1);
}
#endif

TEST_CASE("Duration::from(float secs)", "[duration]")
{
    // durations can be created from a float
    auto d1 = Duration::from(1234.5456f);
    nel::unused(d1);
}

TEST_CASE("Duration::from(double secs)", "[duration]")
{
    // durations can be created from a double
    auto d1 = Duration::from(1234.5456);
    nel::unused(d1);
}

//=============================================================================
TEST_CASE("Duration::as_micros", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration();
        REQUIRE(d1.as_micros() == 0);
    }
    {
        auto d1 = 124_us;
        REQUIRE(d1.as_micros() == 124);
    }
}

TEST_CASE("Duration::as_millis", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration();
        REQUIRE(d1.as_millis() == 0);
    }
    {
        auto d1 = 124_ms;
        REQUIRE(d1.as_millis() == 124);
    }
}

TEST_CASE("Duration::as_secs", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration();
        REQUIRE(d1.as_secs() == 0);
    }
    {
        auto d1 = 124_s;
        REQUIRE(d1.as_secs() == 124);
    }
}

TEST_CASE("Duration::as_float", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration();
        REQUIRE(d1.as_float() == 0.0f);
    }
    {
        auto d1 = Duration::from(124.0f);
        REQUIRE(d1.as_float() == 124.0f);
    }
}

TEST_CASE("Duration::as_double", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration();
        REQUIRE(d1.as_double() == 0.0);
    }
    {
        auto d1 = Duration::from(124.0);
        REQUIRE(d1.as_double() == 124.0);
    }
}

#if 0
TEST_CASE("Duration::as_timespec", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration();
        REQUIRE(d1.as_timespec() == Timespec {0, 0});
    }
    {
        auto d1 = Duration::from(Timespec {124, 6434});
        REQUIRE(d1.as_timespec() == Timespec {124, 6434});
    }
}
#endif

TEST_CASE("Duration::as_frequency", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = 1_ms;
        REQUIRE(d1.as_frequency() == 1.0_kHz);
    }
}

TEST_CASE("Duration::from(frequency)", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration::from(1.0_kHz);
        REQUIRE(d1 == 1_ms);
    }
}

}; // namespace duration
}; // namespace test
}; // namespace nel
