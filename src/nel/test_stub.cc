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
        CHECK(Stub::dtor == 1);
        CHECK(Stub::instances == 0);
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
            CHECK(Stub::instances == 1);

            auto s2 = move(s1);
            nel::unused(s2);

            // new instance IS created.
            CHECK(Stub::instances == 2);
            // no instances destroyed.
            CHECK(Stub::dtor == 0);

            CHECK(Stub::move_ctor == 1);
            CHECK(Stub::move_assn == 0);
        }
    }
    {
        // stub move-assn does not change instance count
        // as dst location is reused.
        Stub::reset();
        auto s1 = Stub(1);
        auto s2 = Stub(2);
        {
            CHECK(Stub::instances == 2);

            s2 = move(s1);
            nel::unused(s2);

            // no new instances created.
            CHECK(Stub::instances == 2);
            // no instances destroyed.
            CHECK(Stub::dtor == 0);

            CHECK(Stub::move_ctor == 0);
            CHECK(Stub::move_assn == 1);
        }
    }
}

TEST_CASE("Stub::copy", "[stub]")
{
    {
        // stub copy-ctor does change instance count
        Stub::reset();
        auto s1 = Stub(1);
        {
            CHECK(Stub::instances == 1);

            auto s2 = s1;
            nel::unused(s2);

            // copy-ctor caused new instance to be created,
            CHECK(Stub::instances == 2);
            // copy-ctor did not cause a dtuct.
            CHECK(Stub::dtor == 0);

            CHECK(Stub::copy_ctor == 1);
            CHECK(Stub::copy_assn == 0);
        }
    }
    {
        // stub copy-assn does not change instance count
        Stub::reset();
        auto s1 = Stub(1);
        auto s2 = Stub(2);
        {
            CHECK(Stub::instances == 2);

            s2 = s1;
            nel::unused(s2);

            // copy-assn did not cause a new instance to be created.
            CHECK(Stub::instances == 2);
            // copy-assn did not cause a dtuct.
            CHECK(Stub::dtor == 0);

            CHECK(Stub::copy_ctor == 0);
            CHECK(Stub::copy_assn == 1);
        }
    }
}

}; // namespace stub
}; // namespace test
}; // namespace nel
