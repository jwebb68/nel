// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/time/duration.hh>
#include <nel/memory.hh> // move

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace frequency
{

using nel::move;
using nel::time::Duration;
using nel::time::Frequency;

TEST_CASE("Frequency::move", "[frequency]")
{
    // frequencies can be moved.
    {
        auto d1 = Frequency(0UL);
        auto d2 = Frequency(move(d1));

        NEL_UNUSED(d2);
    }

    {
        auto d1 = Frequency(0UL);
        auto d2 = move(d1);

        NEL_UNUSED(d2);
    }
}

TEST_CASE("Frequency::copy", "[frequency]")
{
    // frequencies can be copied.
    {
        auto d1 = Frequency(0UL);
        auto d2 = Frequency(d1);
        NEL_UNUSED(d2);
    }
    {
        auto d1 = Frequency(0UL);
        auto d2 = d1;
        NEL_UNUSED(d2);
    }
}

//============================================================================
TEST_CASE("Frequency::iadd(Freq)", "[frequency]")
{
    // frequencies can be added.
    {
        auto d1 = Frequency(20UL);
        d1 += Frequency(30UL);
        REQUIRE(d1 == Frequency(20UL + 30UL));
    }
}

TEST_CASE("Frequency::add(Freq)", "[frequency]")
{
    // frequencies can be added.
    {
        auto d1 = Frequency(20UL);
        auto d2 = d1 + Frequency(30UL);
        REQUIRE(d2 == Frequency(20UL + 30UL));
    }
}

TEST_CASE("Frequency::isub(Freq)", "[frequency]")
{
    // frequencies can be subtracted.
    // TODO: negative freqs?
    {
        auto d1 = Frequency(30UL);
        d1 -= Frequency(20UL);
        REQUIRE(d1 == Frequency(30UL - 20UL));
    }
}

TEST_CASE("Frequency::sub(Freq)", "[frequency]")
{
    // frequencies can be added.
    // TODO: negative freqs?
    {
        auto d1 = Frequency(30UL);
        auto d2 = d1 - Frequency(20UL);
        REQUIRE(d2 == Frequency(30UL - 20UL));
    }
}

TEST_CASE("Frequency::imul(uint64)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = Frequency(20UL);
        d1 *= 1234UL;
        REQUIRE(d1 == Frequency(20UL * 1234UL));
    }
}

TEST_CASE("Frequency::imul(float)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = Frequency(20UL);
        d1 *= 1234.0f;
        REQUIRE(d1 == Frequency(20 * 1234.0f));
    }
}

TEST_CASE("Frequency::imul(double)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = Frequency(20UL);
        d1 *= 1234.0;
        REQUIRE(d1 == Frequency(20 * 1234.0));
    }
}

TEST_CASE("Frequency::idiv(uint64)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = Frequency(20345UL);
        d1 /= 1234UL;
        REQUIRE(d1 == Frequency(20345UL / 1234UL));
    }
}

TEST_CASE("Frequency::idiv(float)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = Frequency(20345UL);
        d1 /= 1234.0f;
        REQUIRE(d1 == Frequency(20345 / 1234.0f));
    }
}

TEST_CASE("Frequency::idiv(double)", "[frequency]")
{
    // frequency can be multiplied to scale it.
    {
        auto d1 = Frequency(20345UL);
        d1 /= 1234.0;
        REQUIRE(d1 == Frequency(20345 / 1234.0));
    }
}

//============================================================================
TEST_CASE("Frequency::eq", "[frequency]")
{
    // frequency can be compared for equality
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(20UL);
        REQUIRE(d1 == d2);
    }
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(30UL);
        REQUIRE(!(d1 == d2));
    }
}

TEST_CASE("Frequency::ne", "[frequency]")
{
    // frequency can be compared for inequality
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(30UL);
        REQUIRE(d1 != d2);
    }
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(20UL);
        REQUIRE(!(d1 != d2));
    }
}

TEST_CASE("Frequency::lt", "[frequency]")
{
    // frequency can be compared for less than
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(30UL);
        REQUIRE(d1 < d2);
    }
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(20UL);
        REQUIRE(!(d1 < d2));
    }
    {
        auto d1 = Frequency(30UL);
        auto d2 = Frequency(20UL);
        REQUIRE(!(d1 < d2));
    }
}

TEST_CASE("Frequency::gt", "[frequency]")
{
    // frequency can be compared for greater than
    {
        auto d1 = Frequency(30UL);
        auto d2 = Frequency(20UL);
        REQUIRE(d1 > d2);
    }
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(20UL);
        REQUIRE(!(d1 > d2));
    }
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(30UL);
        REQUIRE(!(d1 > d2));
    }
}

TEST_CASE("Frequency::le", "[frequency]")
{
    // frequency can be compared for less than or equal to
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(30UL);
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(20UL);
        REQUIRE(d1 <= d2);
    }
    {
        auto d1 = Frequency(30UL);
        auto d2 = Frequency(20UL);
        REQUIRE(!(d1 <= d2));
    }
}

TEST_CASE("Frequency::ge", "[frequency]")
{
    // frequency can be compared for greater than or equality
    {
        auto d1 = Frequency(30UL);
        auto d2 = Frequency(20UL);
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(20UL);
        REQUIRE(d1 >= d2);
    }
    {
        auto d1 = Frequency(20UL);
        auto d2 = Frequency(30UL);
        REQUIRE(!(d1 >= d2));
    }
}

//============================================================================

TEST_CASE("Frequency::default", "[frequency]")
{
    // frequencies can be created without a value.
    auto d1 = Frequency();
    NEL_UNUSED(d1);
}

//=============================================================================
TEST_CASE("Frequency::as_kilo", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Frequency::from_kilo(0);
        REQUIRE(d1.as_kilo() == 0);
    }
    {
        auto d1 = Frequency::from_kilo(124);
        REQUIRE(d1.as_kilo() == 124);
    }
}

TEST_CASE("Frequency::as_mega", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Frequency::from_mega(0);
        REQUIRE(d1.as_mega() == 0);
    }
    {
        auto d1 = Frequency::from_mega(124);
        REQUIRE(d1.as_mega() == 124);
    }
}

TEST_CASE("Frequency::as_unit", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Frequency::from_unit(0);
        REQUIRE(d1.as_unit() == 0);
    }
    {
        auto d1 = Frequency(124UL);
        REQUIRE(d1.as_unit() == 124);
    }
}

TEST_CASE("Frequency::as_float", "[frequency]")
{
    {
        auto d1 = Frequency::from_unit(0);
        REQUIRE(d1.as_float() == 0.0f);
    }
    {
        auto d1 = Frequency(124.0f);
        REQUIRE(d1.as_float() == 124.0f);
    }
}

TEST_CASE("Frequency::as_double", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Frequency::from_unit(0);
        REQUIRE(d1.as_double() == 0.0);
    }
    {
        auto d1 = Frequency(124.0);
        REQUIRE(d1.as_double() == 124.0);
    }
}

TEST_CASE("Frequency::as_duration", "[frequency]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Frequency::from_kilo(1);
        REQUIRE(d1.as_duration() == Duration::from_millis(1));
    }
}

TEST_CASE("Frequency::from(duration)", "[duration]")
{
    {
        // default could be unset
        // default is 0 for simplicity.
        auto d1 = Frequency::from(Duration::from_millis(1));
        REQUIRE(d1 == Frequency::from_kilo(1));
    }
}

}; // namespace frequency
}; // namespace test
}; // namespace nel
