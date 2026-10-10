// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "duration.hh"

#include <nel/memory.hh> // move
#include <nel/defs.hh> // unused
#include <nel/types.hh> // u64

#include <catch2/catch.hpp>
#include <catch2/catch2_panic.hh>

namespace nel
{
namespace test
{
namespace frequency
{

using nel::time::Duration;
using nel::time::Frequency;

TEST_CASE("Frequency::move", "[frequency]")
{
    // frequencies can be moved.
    {
        auto d1 = 0.0_Hz;
        auto d2 = move(d1);

        nel::unused(d2);
    }
}

TEST_CASE("Frequency::copy", "[frequency]")
{
    // frequencies can be copied.
    {
        auto d1 = 0.0_Hz;
        auto d2 = d1;
        nel::unused(d2);
    }
}

//============================================================================
TEST_CASE("Frequency::iadd(Freq)", "[frequency]")
{
    // frequencies can be added.
    {
        auto d1 = 20.0_Hz;
        d1 += 30.0_Hz;
        REQUIRE(d1 == (20.0_Hz + 30.0_Hz));
    }
}

TEST_CASE("Frequency::add(Freq)", "[frequency]")
{
    // frequencies can be added.
    {
        auto d1 = 20.0_Hz;
        auto d2 = d1 + 30.0_Hz;
        REQUIRE(d2 == (20.0_Hz + 30.0_Hz));
    }
}

TEST_CASE("Frequency::isub(Freq)", "[frequency]")
{
    // frequencies can be subtracted.
    // TODO: negative freqs?
    {
        auto d1 = 30.0_Hz;
        d1 -= 20.0_Hz;
        REQUIRE(d1 == (30.0_Hz - 20.0_Hz));
    }
}

TEST_CASE("Frequency::sub(Freq)", "[frequency]")
{
    // frequencies can be added.
    // TODO: negative freqs?
    {
        auto d1 = 30.0_Hz;
        auto d2 = d1 - 20.0_Hz;
        REQUIRE(d2 == 30.0_Hz - 20.0_Hz);
    }
}

TEST_CASE("Frequency::imul(uint64)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = 20.0_Hz;
        d1 *= 1234_u64;
        auto v = 24680.0_Hz;
        REQUIRE(d1 == v);
    }
}

TEST_CASE("Frequency::imul(float)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = 20.0_Hz;
        d1 *= 1234.0f;
        auto v = 24680.0_Hz;
        REQUIRE(d1 == v);
    }
}

TEST_CASE("Frequency::imul(double)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = 20.0_Hz;
        d1 *= 1234.0;
        auto v = 24680.0_Hz;
        REQUIRE(d1 == v);
    }
}

TEST_CASE("Frequency::idiv(uint64)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = 2468.0_Hz;
        d1 /= 2_u64;
        auto v  = 1234.0_Hz;
        REQUIRE(d1 == v);
    }
}

TEST_CASE("Frequency::idiv(float)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = 2468.0_Hz;
        d1 /= 2.0f;
        auto v = 1234.0_Hz;
        REQUIRE(d1 == v);
    }
}

TEST_CASE("Frequency::idiv(double)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = 2468.0_Hz;
        d1 /= 2.0;
        auto v = 1234.0_Hz;
        REQUIRE(d1 == v);
    }
}

//============================================================================
TEST_CASE("Frequency::eq", "[frequency]")
{
    // frequency can be compared for equality
    {
        auto d1 = 20.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE(d1 == d2);
    }
    {
        auto d1 = 20.0_Hz;
        auto d2 = 30.0_Hz;
        REQUIRE_FALSE(d1 == d2);
    }
}

TEST_CASE("Frequency::ne", "[frequency]")
{
    // frequency can be compared for inequality
    {
        auto d1 = 20.0_Hz;
        auto d2 = 30.0_Hz;
        REQUIRE(d1 != d2);
    }
    {
        auto d1 = 20.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE_FALSE(d1 != d2);
    }
}

TEST_CASE("Frequency::lt", "[frequency]")
{
    // frequency can be compared for less than
    {
        auto d1 = 20.0_Hz;
        auto d2 = 30.0_Hz;
        REQUIRE(d1 < d2);
    }
    {
        auto d1 = 20.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE_FALSE(d1 < d2);
    }
    {
        auto d1 = 30.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE_FALSE(d1 < d2);
    }
}

TEST_CASE("Frequency::gt", "[frequency]")
{
    // frequency can be compared for greater than
    {
        auto d1 = 30.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE(d1 > d2);
    }
    {
        auto d1 = 20.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE_FALSE(d1 > d2);
    }
    {
        auto d1 = 20.0_Hz;
        auto d2 = 30.0_Hz;
        REQUIRE_FALSE(d1 > d2);
    }
}

TEST_CASE("Frequency::le", "[frequency]")
{
    // frequency can be compared for less than or equal to
    {
        auto d1 = 20.0_Hz;
        auto d2 = 30.0_Hz;
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = 20.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = 30.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE_FALSE(d1 <= d2);
    }
}

TEST_CASE("Frequency::ge", "[frequency]")
{
    // frequency can be compared for greater than or equality
    {
        auto d1 = 30.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = 20.0_Hz;
        auto d2 = 20.0_Hz;
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = 20.0_Hz;
        auto d2 = 30.0_Hz;
        REQUIRE_FALSE(d1 >= d2);
    }
}

//============================================================================

TEST_CASE("Frequency::default", "[frequency]")
{
    // frequencies can be created without a value.
    auto d1 = Frequency();
    nel::unused(d1);
}

//=============================================================================
TEST_CASE("Frequency::as_kilo", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = 0.0_kHz;
        REQUIRE(d1.as_kilo() == 0);
    }
    {
        auto d1 = 124.0_kHz;
        REQUIRE(d1.as_kilo() == 124);
    }
}

TEST_CASE("Frequency::as_mega", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = 0.0_MHz;
        REQUIRE(d1.as_mega() == 0);
    }
    {
        auto d1 = 124.0_MHz;
        REQUIRE(d1.as_mega() == 124);
    }
}

TEST_CASE("Frequency::as_unit", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = 0.0_Hz;
        REQUIRE(d1.as_unit() == 0);
    }
    {
        auto d1 = 124.0_Hz;
        REQUIRE(d1.as_unit() == 124);
    }
}

TEST_CASE("Frequency::as_float", "[frequency]")
{
    {
        auto d1 = 0.0_Hz;
        REQUIRE(d1.as_float() == 0.0f);
    }
    {
        auto d1 = 124.0_Hz;
        REQUIRE(d1.as_float() == 124.0f);
    }
}

TEST_CASE("Frequency::as_double", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = 0.0_Hz;
        REQUIRE(d1.as_double() == 0.0);
    }
    {
        auto d1 = 124.0_Hz;
        REQUIRE(d1.as_double() == 124.0);
    }
}

TEST_CASE("Frequency::as_duration", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = 1.0_kHz;
        REQUIRE(d1.as_duration() == 1.0_ms);
    }
}

TEST_CASE("Frequency::from(duration)", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Frequency::from(1.0_ms);
        REQUIRE(d1 == 1.0_kHz);
    }
}

}; // namespace frequency
}; // namespace test
}; // namespace nel
