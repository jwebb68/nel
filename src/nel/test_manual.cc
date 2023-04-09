// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "manual.hh"

#include <catch2/catch.hpp>

namespace test
{
namespace manual
{

struct Stub
{
        static int instances;
        static int move_ctor;
        static int move_assn;

        static void reset()
        {
            instances = 0;
            move_ctor = 0;
            move_assn = 0;
        }

        int val;
        bool valid;

        ~Stub()
        {
            if (valid) { instances -= 1; }
        }

        Stub(int v)
            : val(v)
            , valid(true)
        {
            instances += 1;
        }

        Stub(Stub &&o)
            : val(nel::move(o.val))
            , valid(nel::move(o.valid))
        {
            o.valid = false;
            move_ctor += 1;
        }

        Stub &operator=(Stub &&o)
        {
            val = nel::move(o.val);
            valid = nel::move(o.valid);
            o.valid = false;
            move_assn += 1;
            return *this;
        }

        Stub(Stub const &o) = delete;

        // no default ctor..
};

int Stub::instances = 0;
int Stub::move_ctor = 0;
int Stub::move_assn = 0;

TEST_CASE("manual::dtor", "[manual]")
{
    Stub::reset();
    {
        // creating a manual<T> does not create a T.
        nel::Manual<Stub> m;
        nel::unused(m);
        // no Ts are created
        REQUIRE(Stub::instances == 0);
    }

    // no Ts are destructed
    REQUIRE(Stub::instances == 0);

    Stub::reset();
    {
        nel::Manual<Stub> m(Stub(1));

        REQUIRE(Stub::instances == 1);
    }
    // no Ts are destructed
    REQUIRE(Stub::instances == 1);
}

TEST_CASE("manual::move-tor", "[manual]")
{
    {
        Stub::reset();
        // creating a manual<T> does not create a T.
        auto m1 = nel::Manual<Stub>();
        auto m2 = nel::move(m1);

        // no Ts are created
        REQUIRE(Stub::instances == 0);
        // no T moves are performed
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
    }

    {
        Stub::reset();
        auto m1 = nel::Manual<Stub>(Stub(1));
        auto m2 = nel::move(m1);

        REQUIRE(Stub::instances == 1);
        // no T moves are performed
        // note: the move into the container will count as 1 move.
        REQUIRE(Stub::move_ctor == 1);
        REQUIRE(Stub::move_assn == 0);
    }
}

TEST_CASE("manual::move-assgn", "[manual]")
{
    {
        // move empty onto empty
        Stub::reset();
        // creating a manual<T> does not create a T.
        auto m1 = nel::Manual<Stub>();
        auto m2 = nel::Manual<Stub>();

        m2 = nel::move(m1);

        // no Ts are created
        REQUIRE(Stub::instances == 0);
        // no T moves are performed
        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 0);
    }

    {
        // move containing onto empty
        Stub::reset();

        auto m1 = nel::Manual<Stub>(Stub(1));
        auto m2 = nel::Manual<Stub>();

        m2 = nel::move(m1);

        REQUIRE(Stub::instances == 1);
        // no T moves are performed
        // note the move into the container will count as 1 move
        REQUIRE(Stub::move_ctor == 1);
        REQUIRE(Stub::move_assn == 0);
    }

    {
        // move containing onto containing
        Stub::reset();

        auto m1 = nel::Manual<Stub>(Stub(1));
        auto m2 = nel::Manual<Stub>(Stub(2));

        m2 = nel::move(m1);

        REQUIRE(Stub::instances == 2);
        // no extra  T moves are performed
        // note: the move into the container will count as 1 move
        REQUIRE(Stub::move_ctor == 2);
        REQUIRE(Stub::move_assn == 0);

        REQUIRE((*m1).val == 1);
        // and m2 has old value, not new..
        REQUIRE((*m2).val == 2);
    }

    {
        // move empty onto containing
        Stub::reset();

        auto m1 = nel::Manual<Stub>();
        auto m2 = nel::Manual<Stub>(Stub(2));

        m2 = nel::move(m1);

        REQUIRE(Stub::instances == 1);
        // no T moves are performed
        // note: the move into the container will count as 1 move
        REQUIRE(Stub::move_ctor == 1);
        REQUIRE(Stub::move_assn == 0);

        // and m2 has old value, not new..
        REQUIRE((*m2).val == 2);
    }
}

TEST_CASE("manual::destroy", "[manual]")
{
    {
        // deletes contained value
        // UB if no value contained.
        // UB if value already deleted.
        Stub::reset();
        auto m1 = nel::Manual<Stub>();
        m1 = nel::move(Stub(2));

        REQUIRE(Stub::instances == 1);
        m1.destroy();

        REQUIRE(Stub::instances == 0);
    }
}

TEST_CASE("manual::move-in", "[manual]")
{
    {
        // value can be moved in.
        Stub::reset();
        auto m1 = nel::Manual<Stub>();
        m1 = nel::move(Stub(2));

        REQUIRE(Stub::instances == 1);

        REQUIRE(Stub::move_ctor == 0);
        REQUIRE(Stub::move_assn == 1);
        REQUIRE((*m1).val == 2);
    }

    {
        // value can be moved out.
        Stub::reset();
        auto m1 = nel::Manual<Stub>();
        m1 = nel::move(Stub(2));

        auto t = nel::move(*m1);

        REQUIRE(Stub::instances == 1);

        // move-assn of stub into m1.
        REQUIRE(Stub::move_assn == 1);
        // move-ct of value in m1 to t.
        REQUIRE(Stub::move_ctor == 1);
        REQUIRE(t.val == 2);
        REQUIRE(t.valid);
        // value in m1 invalidated by move-out
        REQUIRE(!(*m1).valid);
    }

    {
        // value can be moved in.
        // moving in twice does not cause the old value dtor to be called
        Stub::reset();
        auto m1 = nel::Manual<Stub>();
        m1 = nel::move(Stub(2));
        m1 = nel::move(Stub(3));

        // note: move-in overwrites and causes leaks..
        // so remember to delete any previous value.
        REQUIRE(Stub::instances == 2);

        // move-assn of stubs into m1.
        REQUIRE(Stub::move_assn == 2);

        // not moved out
        REQUIRE(Stub::move_ctor == 0);

        REQUIRE((*m1).valid);
        REQUIRE((*m1).val == 3);
    }

    {
        // value can be moved in.
        // moving in twice does not cause the old value dtor to be called
        // call .destroy() first..
        Stub::reset();
        auto m1 = nel::Manual<Stub>();
        m1 = nel::move(Stub(2));
        m1.destroy();

        m1 = nel::move(Stub(3));
        REQUIRE(Stub::instances == 1);

        // move-assn of stubs into m1.
        REQUIRE(Stub::move_assn == 2);

        // not moved out.
        REQUIRE(Stub::move_ctor == 0);

        REQUIRE((*m1).valid);
        REQUIRE((*m1).val == 3);
    }
}

}; // namespace manual
}; // namespace test
