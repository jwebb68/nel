// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "manual.hh"
#include "stub.hh"

#include <catch2/catch.hpp>

namespace test
{
namespace manual
{

using nel::test::Stub;

TEST_CASE("~Manual<T>()", "[manual]")
{
    Stub::reset();
    {
        // creating a manual<T> does not create a T.
        auto m = nel::Manual<Stub>();
        nel::unused(m);
        // no Ts are created
        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::dtor == 0);
    }

    // no Ts are destructed
    REQUIRE(Stub::instances == 0);
    REQUIRE(Stub::dtor == 0);

    {
        // copied in.
        auto m = nel::Manual(Stub(1));
        Stub::reset();

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::dtor == 0);
    }
    // no Ts are destructed
    REQUIRE(Stub::dtor == 0);
    REQUIRE(Stub::instances == 0);
}

TEST_CASE("Manual<T>(Manual<T> &&rhs)", "[manual]")
{
    {
        // creating a manual<T> does not create a T.
        auto m1 = nel::Manual<Stub>();
        Stub::reset();

        auto m2 = nel::move(m1);

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
    }

    {
        auto m1 = nel::Manual<Stub>(Stub(1));
        Stub::reset();
        auto m2 = nel::move(m1);

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
    }
}

TEST_CASE("operator=(Manual<T> &&rhs)", "[manual]")
{
    {
        // creating a manual<T> does not create a T.
        auto m1 = nel::Manual<Stub>();
        auto m2 = nel::Manual<Stub>();
        Stub::reset();

        m2 = nel::move(m1);

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
    }

    {
        auto m1 = nel::Manual<Stub>(Stub(1));
        auto m2 = nel::Manual<Stub>();

        Stub::reset();

        m2 = nel::move(m1);

        REQUIRE(Stub::instances == 0);
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
    }
}

TEST_CASE("Manual<T>::destruct()", "[manual]")
{
    {
        // destroys contained value
        // UB if no value contained.
        // UB if value already deleted.
        auto m1 = nel::Manual(Stub(100));

        Stub::reset();

        m1.destruct();

        REQUIRE(Stub::instances == -1);
        REQUIRE(Stub::dtor == 1);
    }
}

TEST_CASE("Manual<T>::construct()", "[manual]")
{
    /**
     * create value in-place
     */

    auto m1 = nel::Manual(Stub(100));

    REQUIRE(*m1 == Stub(100));
}

TEST_CASE("Manual<T>::deref()", "[manual]")
{
    SECTION("mutable")
    {
        auto m1 = nel::Manual(Stub(100));

        REQUIRE(m1.deref() == Stub(100));
    }

    SECTION("const")
    {
        auto const m1 = nel::Manual(Stub(100));

        REQUIRE(m1.deref() == Stub(100));
    }
}

TEST_CASE("*Manual<T>", "[manual]")
{
    SECTION("mutable")
    {
        auto m1 = nel::Manual(Stub(100));

        REQUIRE(*m1 == Stub(100));
    }

    SECTION("const")
    {
        auto const m1 = nel::Manual(Stub(100));

        REQUIRE(*m1 == Stub(100));
    }
}

TEST_CASE("Manual<T>->", "[manual]")
{
    SECTION("mutable")
    {
        auto m1 = nel::Manual(Stub(100));
        Stub::reset();

        m1->foo();

        REQUIRE(Stub::foo_calls == 1);
    }

    SECTION("const")
    {
        auto const m1 = nel::Manual(Stub(100));

        Stub::reset();

        m1->foo();

        REQUIRE(Stub::foo_calls == 1);
    }
}

}; // namespace manual
}; // namespace test
