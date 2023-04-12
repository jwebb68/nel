// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/stub.hh>

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace stub
{

TEST_CASE("Stub::destroy ", "[stub]")
{
    {
        // stub create and destroy gives no instances left over
        Stub::reset();
        {
            Stub s(1);
            nel::unused(s);
        }
        REQUIRE(Stub::instances == 0);
    }
}

TEST_CASE("Stub::move", "[stub]")
{
    {
        // stub move-ctor does change instance count, while src is in-scope
        // but restores it when src goes out of scope.
        // (location instance tracking, not value instance tracking)
        Stub::reset();
        auto s1 = Stub(1);
        {
            auto s2 = move(s1);
            // s2 deleted when scope ends
            nel::unused(s2);
            REQUIRE(Stub::instances == 2);
        }
        REQUIRE(Stub::instances == 1);
    }
    {
        // stub move-assn does not change instance count
        // as dst location is reused.
        Stub::reset();
        auto s1 = Stub(1);
        auto s2 = Stub(2);
        {
            s2 = move(s1);
            // s2 deleted when scope ends
            nel::unused(s2);
            REQUIRE(Stub::instances == 2);
        }
        REQUIRE(Stub::instances == 2);
    }
}

TEST_CASE("Stub::copy", "[stub]")
{
    {
        // stub copy-ctor does change instance count
        Stub::reset();
        auto s1 = Stub(1);
        {
            auto s2 = s1;
            // s2 deleted when scope ends
            nel::unused(s2);
            REQUIRE(Stub::instances == 2);
        }
        REQUIRE(Stub::instances == 1);
    }
    {
        // stub copy-assn does not change instance count
        Stub::reset();
        auto s1 = Stub(1);
        auto s2 = Stub(2);
        {
            s2 = s1;
            // s2 deleted when scope ends
            nel::unused(s2);
            REQUIRE(Stub::instances == 2);
        }
        REQUIRE(Stub::instances == 2);
    }
}

}; // namespace stub
}; // namespace test
}; // namespace nel
