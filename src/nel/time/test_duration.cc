// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/time/duration.hh>
#include <nel/memory.hh> // move

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace duration
{

using nel::move;
using nel::time::Duration;
using nel::time::Frequency;

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

//============================================================================

TEST_CASE("Duration::move", "[duration]")
{
    // durations can be moved.
    {
        auto d1 = Duration();
        auto d2 = Duration(move(d1));

        NEL_UNUSED(d2);
    }

    {
        auto d1 = Duration();
        auto d2 = move(d1);

        NEL_UNUSED(d2);
    }
}

TEST_CASE("Duration::copy", "[duration]")
{
    // durations can be copied.
    {
        auto d1 = Duration();
        auto d2 = Duration(d1);
        NEL_UNUSED(d2);
    }
    {
        auto d1 = Duration();
        auto d2 = d1;
        NEL_UNUSED(d2);
    }
}

//============================================================================
TEST_CASE("Duration::iadd", "[duration]")
{
    // duration can be added to another
    auto d1 = Duration::from_micros(20);
    auto d2 = Duration::from_micros(30);
    d1 += d2;
    REQUIRE(d1 == Duration::from_micros(50));
}

TEST_CASE("Duration::add", "[duration]")
{
    // duration can be added to another
    auto d1 = Duration::from_micros(20);
    auto d2 = Duration::from_micros(30);
    auto d3 = d1 + d2;
    REQUIRE(d3 == Duration::from_micros(50));
}

TEST_CASE("Duration::isub", "[duration]")
{
    // duration can be subtracted from another
    {
        auto d1 = Duration::from_micros(20);
        auto d2 = Duration::from_micros(10);
        d1 -= d2;
        REQUIRE(d1 == Duration::from_micros(10));
    }
    {
        // and can go negative. ?
        auto d1 = Duration::from_micros(20);
        auto d2 = Duration::from_micros(50);
        d1 -= d2;
        REQUIRE(d1 == Duration::from_micros(-30));
    }
}

TEST_CASE("Duration::sub", "[duration]")
{
    // duration can be subtracted from another
    {
        auto d1 = Duration::from_micros(20);
        auto d2 = Duration::from_micros(10);
        auto d3 = d1 - d2;
        REQUIRE(d3 == Duration::from_micros(10));
    }
    {
        // and can go negative. ?
        auto d1 = Duration::from_micros(20);
        auto d2 = Duration::from_micros(50);
        auto d3 = d1 - d2;
        REQUIRE(d3 == Duration::from_micros(-30));
    }
}

TEST_CASE("Duration::imul(uint64)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20);
        d1 *= 1234UL;
        REQUIRE(d1 == Duration::from_micros(20 * 1234));
    }
}

TEST_CASE("Duration::imul(float)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20);
        d1 *= 1234.0f;
        REQUIRE(d1 == Duration::from_micros(20 * 1234.0f));
    }
}

TEST_CASE("Duration::imul(double)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20);
        d1 *= 1234.0;
        REQUIRE(d1 == Duration::from_micros(20 * 1234.0));
    }
}

TEST_CASE("Duration::mul(uint64)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20);
        auto d2 = d1 * 1234UL;
        REQUIRE(d2 == Duration::from_micros(20 * 1234));
    }
}

TEST_CASE("Duration::mul(float)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20);
        auto d2 = d1 * 1234.0f;
        REQUIRE(d2 == Duration::from_micros(20 * 1234.0f));
    }
}

TEST_CASE("Duration::mul(double)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20);
        auto d2 = d1 * 1234.0;
        REQUIRE(d2 == Duration::from_micros(20 * 1234.0));
    }
}

TEST_CASE("Duration::idiv(uint64)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20345);
        d1 /= 1234UL;
        REQUIRE(d1 == Duration::from_micros(20345UL / 1234UL));
        // auto v1 = d1.as_micros();
        // auto v2 = 20345UL/1234UL;
        // REQUIRE(v1 == v2);
    }
}

TEST_CASE("Duration::idiv(float)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20345);
        d1 /= 1234.0f;
        REQUIRE(d1 == Duration::from_micros(20345 / 1234.0f));
    }
}

TEST_CASE("Duration::idiv(double)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20345);
        d1 /= 1234.0;
        REQUIRE(d1 == Duration::from_micros(20345 / 1234.0));
    }
}

TEST_CASE("Duration::div(uint64)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20345);
        auto d2 = d1 / 1234UL;
        REQUIRE(d2 == Duration::from_micros(20345UL / 1234UL));
        // auto v1 = d1.as_micros();
        // auto v2 = 20345UL/1234UL;
        // REQUIRE(v1 == v2);
    }
}

TEST_CASE("Duration::div(float)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20345);
        auto d2 = d1 / 1234.0f;
        REQUIRE(d2 == Duration::from_micros(20345 / 1234.0f));
    }
}

TEST_CASE("Duration::div(double)", "[duration]")
{
    // duration can be multiplied to scale it.
    {
        auto d1 = Duration::from_micros(20345);
        auto d2 = d1 / 1234.0;
        REQUIRE(d2 == Duration::from_micros(20345 / 1234.0));
    }
}

//============================================================================
TEST_CASE("Duration::eq", "[duration]")
{
    // duration can be compared for equality
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(20);
        REQUIRE(d1 == d2);
    }
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(30);
        REQUIRE(!(d1 == d2));
    }
}

TEST_CASE("Duration::ne", "[duration]")
{
    // duration can be compared for inequality
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(30);
        REQUIRE(d1 != d2);
    }
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(20);
        REQUIRE(!(d1 != d2));
    }
}

TEST_CASE("Duration::lt", "[duration]")
{
    // duration can be compared for less than
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(30);
        REQUIRE(d1 < d2);
    }
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(20);
        REQUIRE(!(d1 < d2));
    }
    {
        auto d1 = Duration::from_millis(30);
        auto d2 = Duration::from_millis(20);
        REQUIRE(!(d1 < d2));
    }
}

TEST_CASE("Duration::gt", "[duration]")
{
    // duration can be compared for greater than
    {
        auto d1 = Duration::from_millis(30);
        auto d2 = Duration::from_millis(20);
        REQUIRE(d1 > d2);
    }
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(20);
        REQUIRE(!(d1 > d2));
    }
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(30);
        REQUIRE(!(d1 > d2));
    }
}

TEST_CASE("Duration::le", "[duration]")
{
    // duration can be compared for less than or equal to
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(30);
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(20);
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = Duration::from_millis(30);
        auto d2 = Duration::from_millis(20);
        REQUIRE(!(d1 <= d2));
    }
}

TEST_CASE("Duration::ge", "[duration]")
{
    // duration can be compared for greater than or equality
    {
        auto d1 = Duration::from_millis(30);
        auto d2 = Duration::from_millis(20);
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(20);
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = Duration::from_millis(20);
        auto d2 = Duration::from_millis(30);
        REQUIRE(!(d1 >= d2));
    }
}

//============================================================================

TEST_CASE("Duration::default", "[duration]")
{
    // durations can be created without a value.
    auto d1 = Duration();
    NEL_UNUSED(d1);
}

TEST_CASE("Duration::from_micros", "[duration]")
{
    // durations can be created from a  micros value
    auto d1 = Duration::from_micros(1234);
    NEL_UNUSED(d1);
}

TEST_CASE("Duration::from_millis", "[duration]")
{
    // durations can be created from a millis value
    auto d1 = Duration::from_millis(1234);
    NEL_UNUSED(d1);
}

TEST_CASE("Duration::from_secs", "[duration]")
{
    // durations can be created from a secs value
    auto d1 = Duration::from_secs(1234);
    NEL_UNUSED(d1);
}

TEST_CASE("Duration::from(secs, usecs)", "[duration]")
{
    // durations can be created from a secs+usecs value
    auto d1 = Duration::from(1234, 5678);
    NEL_UNUSED(d1);
}

TEST_CASE("Duration::from(Timespec)", "[duration]")
{
    // durations can be created from a Timespec
    auto d1 = Duration::from(Timespec {1234, 5456});
    NEL_UNUSED(d1);
}

TEST_CASE("Duration::from(float secs)", "[duration]")
{
    // durations can be created from a float
    auto d1 = Duration::from(1234.5456f);
    NEL_UNUSED(d1);
}

TEST_CASE("Duration::from(double secs)", "[duration]")
{
    // durations can be created from a float
    auto d1 = Duration::from(1234.5456);
    NEL_UNUSED(d1);
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
        auto d1 = Duration::from_micros(124);
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
        auto d1 = Duration::from_millis(124);
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
        auto d1 = Duration::from_secs(124);
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

TEST_CASE("Duration::as_frequency", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration::from_millis(1);
        REQUIRE(d1.as_frequency() == Frequency::from_kilo(1));
    }
}

TEST_CASE("Duration::from(frequency)", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Duration::from(Frequency::from_kilo(1));
        REQUIRE(d1 == Duration::from_millis(1));
    }
}

}; // namespace duration
}; // namespace test
}; // namespace nel
