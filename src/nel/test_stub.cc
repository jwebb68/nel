// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/stub.hh>

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace stub
{

TEST_CASE("Stub::destroy", "[stub]")
{
    /**
     * destructor increases dtor count and decreases  instance count.
     * and nothing else.
     */
    {
        auto s = Stub(100);
        Stub::reset();
    }

    REQUIRE(Stub::instances == -1);
    REQUIRE(Stub::ctor == 0);
    REQUIRE(Stub::dtor == 1);
    REQUIRE(Stub::move_ctor == 0);
    REQUIRE(Stub::move_assn == 0);
    REQUIRE(Stub::copy_ctor == 0);
    REQUIRE(Stub::copy_assn == 0);
}

TEST_CASE("Stub::move-ctor", "[stub]")
{
    /**
     * move-ctor does not increase instance count
     * move-ctor increases move_ctor  count.
     * and nothing else.
     */
    SECTION("rhs valid")
    {
        auto s = Stub(100);
        Stub::reset();
        auto s2 = nel::move(s);

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::ctor == 0);
        REQUIRE(Stub::dtor == 0);
        REQUIRE(Stub::move_ctor == 1);
        REQUIRE(Stub::move_assn == 0);
        REQUIRE(Stub::copy_ctor == 0);
        REQUIRE(Stub::copy_assn == 0);

        REQUIRE(!s.valid);
        REQUIRE(s2.valid);
    }

    SECTION("rhs invalid")
    {
        auto s = Stub(100);
        auto s1 = nel::move(s);
        Stub::reset();
        auto s2 = nel::move(s);

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::ctor == 0);
        REQUIRE(Stub::dtor == 0);
        REQUIRE(Stub::move_ctor == 1);
        REQUIRE(Stub::move_assn == 0);
        REQUIRE(Stub::copy_ctor == 0);
        REQUIRE(Stub::copy_assn == 0);

        REQUIRE(!s.valid);
        // invalid is moved with value.
        REQUIRE(!s2.valid);
    }
}

TEST_CASE("Stub::move-assn", "[stub]")
{
    /**
     * move-assgn does not increase instance count
     * move-assgn increases move_assn count.
     * and nothing else.
     */
    SECTION("rhs valid")
    {
        auto s = Stub(100);
        auto s2 = Stub(200);
        Stub::reset();
        s2 = nel::move(s);

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::ctor == 0);
        REQUIRE(Stub::dtor == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 1);
        REQUIRE(Stub::copy_ctor == 0);
        REQUIRE(Stub::copy_assn == 0);

        REQUIRE(!s.valid);
        REQUIRE(s2.valid);
    }

    SECTION("rhs invalid")
    {
        auto s = Stub(100);
        auto s2 = Stub(200);
        auto s1 = nel::move(s);
        Stub::reset();
        s2 = nel::move(s);

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::ctor == 0);
        REQUIRE(Stub::dtor == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 1);
        REQUIRE(Stub::copy_ctor == 0);
        REQUIRE(Stub::copy_assn == 0);

        REQUIRE(!s.valid);
        // invalid is moved with value.
        REQUIRE(!s2.valid);
    }
}

TEST_CASE("Stub::copy-ctor", "[stub]")
{
    /**
     * copy-ctor does increase instance count
     * copy-ctor increases copy_ctor count.
     * and nothing else.
     */
    SECTION("rhs valid")
    {
        auto s = Stub(100);
        Stub::reset();
        auto s2 = s;

        REQUIRE(Stub::instances == 1);
        REQUIRE(Stub::ctor == 0);
        REQUIRE(Stub::dtor == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
        REQUIRE(Stub::copy_ctor == 1);
        REQUIRE(Stub::copy_assn == 0);

        REQUIRE(s.valid);
        REQUIRE(s2.valid);
    }

    SECTION("rhs invalid")
    {
        auto s = Stub(100);
        auto s1 = nel::move(s);
        Stub::reset();
        auto s2 = s;

        REQUIRE(Stub::instances == 1);
        REQUIRE(Stub::ctor == 0);
        REQUIRE(Stub::dtor == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
        REQUIRE(Stub::copy_ctor == 1);
        REQUIRE(Stub::copy_assn == 0);

        REQUIRE(!s.valid);
        // invalid is copied with value.
        REQUIRE(!s2.valid);
    }
}

TEST_CASE("Stub::copy-assn", "[stub]")
{
    /**
     * copy-assgn does not increase instance count
     * copy-assgn increases move_assn count.
     * and nothing else.
     */
    SECTION("rhs valid")
    {
        auto s = Stub(100);
        auto s2 = Stub(200);
        Stub::reset();
        s2 = s;

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::ctor == 0);
        REQUIRE(Stub::dtor == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
        REQUIRE(Stub::copy_ctor == 0);
        REQUIRE(Stub::copy_assn == 1);

        REQUIRE(s.valid);
        REQUIRE(s2.valid);
    }

    SECTION("rhs invalid")
    {
        auto s = Stub(100);
        auto s2 = Stub(200);
        auto s1 = nel::move(s);
        Stub::reset();
        s2 = s;

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::ctor == 0);
        REQUIRE(Stub::dtor == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
        REQUIRE(Stub::copy_ctor == 0);
        REQUIRE(Stub::copy_assn == 1);

        REQUIRE(!s.valid);
        // invalid is copied with value.
        REQUIRE(!s2.valid);
    }
}

TEST_CASE("Stub::lhs == Stub::rhs", "[stub]")
{
    SECTION("lhs valid, rhs valid, rhs == lhs")
    {
        auto s1 = Stub(100);
        auto s2 = Stub(100);
        REQUIRE(s1 == s2);
    }
    SECTION("lhs valid, rhs valid, rhs != lhs")
    {
        auto s1 = Stub(100);
        auto s2 = Stub(200);
        REQUIRE(!(s1 == s2));
    }
    SECTION("lhs invalid, rhs valid")
    {
        auto s1 = Stub(100);
        auto s = nel::move(s1);
        auto s2 = Stub(100);
        REQUIRE(!(s1 == s2));
    }
    SECTION("lhs invalid, rhs invalid")
    {
        auto s1 = Stub(100);
        auto s = nel::move(s1);
        auto s2 = Stub(100);
        REQUIRE(!(s1 == s2));
    }
    SECTION("lhs valid, rhs invalid")
    {
        auto s1 = Stub(100);
        auto s2 = Stub(100);
        auto s = nel::move(s2);
        REQUIRE(!(s1 == s2));
    }
}

TEST_CASE("Stub::lhs != Stub::rhs", "[stub]")
{
    SECTION("lhs valid, rhs valid, lhs == rhs")
    {
        auto s1 = Stub(100);
        auto s2 = Stub(100);
        REQUIRE(!(s1 != s2));
    }
    SECTION("lhs valid, rhs valid, lhs != rhs")
    {
        auto s1 = Stub(100);
        auto s2 = Stub(200);
        REQUIRE(s1 != s2);
    }
    SECTION("lhs invalid, rhs valid")
    {
        auto s1 = Stub(100);
        auto s = nel::move(s1);
        auto s2 = Stub(100);
        REQUIRE(!(s1 != s2));
    }
    SECTION("lhs invalid, rhs invalid")
    {
        auto s1 = Stub(100);
        auto s = nel::move(s1);
        auto s2 = Stub(100);
        auto s3 = nel::move(s2);
        REQUIRE(!(s1 != s2));
    }
    SECTION("lhs valid, rhs invalid")
    {
        auto s1 = Stub(100);
        auto s2 = Stub(100);
        auto s = nel::move(s2);
        REQUIRE(!(s1 != s2));
    }
}

TEST_CASE("Stub::foo()", "[stub]")
{
    SECTION("valid")
    {
        auto s1 = Stub(100);
        Stub::reset();
        s1.foo();
        REQUIRE(Stub::foo_calls == 1);
    }
    SECTION("invalid")
    {
        auto s1 = Stub(100);
        Stub::reset();
        s1.foo();
        REQUIRE(Stub::foo_calls == 1);
    }
}

} // namespace stub
} // namespace test
} // namespace nel
