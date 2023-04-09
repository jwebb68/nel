// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/heaped/rc.hh>

#include <nel/memory.hh> // move()

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace heaped
{
namespace rc
{

TEST_CASE("heaped::RC::ctor", "[heaped][rc]")
{
    auto a1 = nel::heaped::RC<int>(1);
    REQUIRE(*a1 == 1);
}

TEST_CASE("heaped::RC::copy", "[heaped][rc]")
{
    // copying does not remove the value
    auto a1 = nel::heaped::RC<int>(1);
    auto a2 = a1;

    REQUIRE(*a1 == 1);
    REQUIRE(*a2 == 1);
}

TEST_CASE("heaped::RC::move", "[heaped][rc]")
{
    auto a1 = nel::heaped::RC<int>(1);
    auto a2 = nel::move(a1);

    REQUIRE(!a1.has_value());
    REQUIRE(*a2 == 1);
}

TEST_CASE("heaped::RC::ref", "[heaped][rc]")
{
    auto a1 = nel::heaped::RC<int>(1);
    REQUIRE(*a1 == 1);
}

TEST_CASE("heaped::RC::has_value", "[heaped][rc]")
{
    // rc has value initially
    auto a1 = nel::heaped::RC<int>(1);
    REQUIRE(a1.has_value());

    // copying does not remove the value
    auto a2 = a1;
    REQUIRE(a1.has_value());
    REQUIRE(a2.has_value());

    // moving removes the value.
    auto a3 = nel::move(a1);
    REQUIRE(a3.has_value());
    REQUIRE(!a1.has_value());
}

TEST_CASE("heaped::RC::unwrap", "[heaped][rc]")
{
    // unwrapping returns the value in the RC.
    auto a1 = nel::heaped::RC<int>(1);
    REQUIRE(a1.unwrap() == 1);

    // unwrapping removes the value in the RC.
    REQUIRE(!a1.has_value());

    // unwrapping when shared invalidates for all.
    auto a2 = nel::heaped::RC<int>(1);
    // copy to share.
    auto a3 = a2;
    REQUIRE(a2.unwrap() == 1);
    // underlying value is invalidated on unwrap.
    REQUIRE(!a2.has_value());
    REQUIRE(!a3.has_value());
}

// TODO: check that dtor of T is called only when last reference is destroyed..

}; // namespace rc
}; // namespace heaped
}; // namespace test
}; // namespace nel
