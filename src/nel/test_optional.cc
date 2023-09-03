// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "optional.hh"
#include "defs.hh"

#include "stub.hh"

#include <catch2/catch.hpp>
#include <cstring> //strcmp

namespace nel
{
namespace test
{
namespace optional
{

// opt<pod>
// opt<udt>
// opt<void>
// opt<pod*>
// opt<udt*>
// opt<void*>
// opt<nullptr> ?
// opt<pod&>
// opt<udt&>
// 8(9) mut tests
// opt<pod const>
// opt<udt const>
// opt<pod const *>
// opt<udt const *>
// opt<void const *>
// opt<pod const &>
// opt<udt const &>
// 7 const tests + 8 mut = 15 subtests for each method? (volatile?
// opt<pod volatile>
// opt<udt volatile>
// opt<pod volatile const>
// opt<udt volatile const>
// opt<pod volatile const *>
// opt<udt volatile const *>
// opt<void volatile const *>
// opt<pod volatile const &>
// opt<udt volatile const &>
// 7mut + 8const + 2mut-vol + 5const-vol = 24..?

// create
// optional::some(x)

TEST_CASE("Optional::Some", "[optional]")
{
    {
        // pod value: can create optional with pod
        auto o1 = nel::Optional<int>::Some(1);
        nel::unused(o1);
    }

    {
        // udt value: can create optional with pod
        auto o1 = nel::Optional<Stub>::Some(Stub(1));
        nel::unused(o1);
    }

    {
        // void value: can create optional with void
        auto o1 = nel::Optional<void>::Some();
        nel::unused(o1);
    }

    {
        // pod const value: can create optional with pod
        auto o1 = nel::Optional<int const>::Some(1);
        nel::unused(o1);
    }

    {
        // udt const value: can create optional with pod
        auto o1 = nel::Optional<Stub const>::Some(Stub(1));
        nel::unused(o1);
    }

    // {
    //     // void const value: ???
    //     auto o1 = nel::Optional<void>::Some();
    //     nel::unused(o1);
    // }

    {
        // pod ptr: can create optional with pod ptr
        int i1 = 1;
        auto o1 = nel::Optional<int *>::Some(&i1);
        nel::unused(o1);
    }

    {
        // udt ptr: can create optional with pod ptr
        auto s1 = Stub(1);
        auto o1 = nel::Optional<Stub *>::Some(&s1);
        nel::unused(o1);
    }

    {
        // void ptr: can create optional with void ptr
        void *p = nullptr;
        auto o1 = nel::Optional<void *>::Some(&p);
        nel::unused(o1);
    }

    {
        // pod const ptr: can create optional with pod ptr
        int const i1 = 1;
        auto o1 = nel::Optional<int const *>::Some(&i1);
        nel::unused(o1);
    }

    {
        // udt const ptr: can create optional with pod ptr
        auto const s1 = Stub(1);
        auto o1 = nel::Optional<Stub const *>::Some(&s1);
        nel::unused(o1);
    }

    {
        // void const ptr: can create optional with void ptr
        void const *p = nullptr;
        auto o1 = nel::Optional<void const *>::Some(&p);
        nel::unused(o1);
    }

    {
        // pod ref: can create optional with pod ref
        int i1 = 1;
        int &i2 = i1;
        auto o1 = nel::Optional<int &>::Some(i2);

        // and can mutate the original via the ref in the optional...
        o1.unwrap() = 23;
        CHECK(i1 == 23);
    }

    {
        // udt ref: can create optional with pod ptr
        auto s1 = Stub(1);
        auto &s2 = s1;
        auto o1 = nel::Optional<Stub &>::Some(s2);

        // and can mutate the original via the ref in the optional...
        o1.unwrap().val = 23;
        CHECK(s1.val == 23);
    }

    {
        // pod ref: can create optional with pod ref
        int const i1 = 1;
        int const &i2 = i1;
        auto o1 = nel::Optional<int const &>::Some(i2);

        // and cannot mutate the original via the ref in the optional...
        // o1.unwrap() = 23;
        CHECK(i1 == 1);
    }

    {
        // udt const ref: can create optional with pod ptr
        auto const s1 = Stub(1);
        auto const &s2 = s1;
        auto o1 = nel::Optional<Stub const &>::Some(s2);

        // and cannot mutate the original via the ref in the optional...
        // o1.unwrap().val = 23;

        CHECK(s1.val == 1);
    }
}

TEST_CASE("Optional::None", "[optional]")
{
    {
        // pod value: can create optional with pod
        auto o1 = nel::Optional<int>::None();
        nel::unused(o1);
    }

    {
        // udt value: can create optional with pod
        auto o1 = nel::Optional<Stub>::None();
        nel::unused(o1);
    }

    {
        // void value: can create optional with void
        auto o1 = nel::Optional<void>::None();
        nel::unused(o1);
    }

    {
        // pod const value: can create optional with pod
        auto o1 = nel::Optional<int const>::None();
        nel::unused(o1);
    }

    {
        // udt const value: can create optional with pod
        auto o1 = nel::Optional<Stub const>::None();
        nel::unused(o1);
    }

    // {
    //     // void const value: ???
    //     auto o1 = nel::Optional<void>::None();
    //     nel::unused(o1);
    // }

    {
        // pod ptr: can create optional with pod ptr
        auto o1 = nel::Optional<int *>::None();
        nel::unused(o1);
    }

    {
        // udt ptr: can create optional with pod ptr
        auto o1 = nel::Optional<Stub *>::None();
        nel::unused(o1);
    }

    {
        // void ptr: can create optional with void ptr
        auto o1 = nel::Optional<void *>::None();
        nel::unused(o1);
    }

    {
        // pod const ptr: can create optional with pod ptr
        auto o1 = nel::Optional<int const *>::None();
        nel::unused(o1);
    }

    {
        // udt const ptr: can create optional with pod ptr
        auto o1 = nel::Optional<Stub const *>::None();
        nel::unused(o1);
    }

    {
        // void const ptr: can create optional with void ptr
        auto o1 = nel::Optional<void const *>::None();
        nel::unused(o1);
    }

    {
        // pod ref: can create optional with pod ref
        auto o1 = nel::Optional<int &>::None();
        nel::unused(o1);
    }

    {
        // udt ref: can create optional with pod ptr
        auto o1 = nel::Optional<Stub &>::None();
        nel::unused(o1);
    }

    {
        // pod ref: can create optional with pod ref
        auto o1 = nel::Optional<int const &>::None();
        nel::unused(o1);
    }

    {
        // udt const ref: can create optional with pod ptr
        auto o1 = nel::Optional<Stub const &>::None();
        nel::unused(o1);
    }
}

TEST_CASE("Optional(Some) dtor", "[optional]")
{
    {
        // pod value: can dtuct optional with pod
        auto o1 = nel::Optional<int>::Some(1);
        nel::unused(o1);
    }

    Stub::reset();
    {
        // udt value: can create optional with pod
        auto o1 = nel::Optional<Stub>::Some(Stub(1));
        // dtuct of temp in creation.
        CHECK(Stub::dtor == 1);

        nel::unused(o1);
        CHECK(Stub::instances == 1);
    }
    // and udt is destroyed on optional<some> destroy.
    CHECK(Stub::dtor == 1 + 1);
    CHECK(Stub::instances == 0);

    {
        // void value: can create optional with void
        auto o1 = nel::Optional<void>::Some();
        nel::unused(o1);
    }

    {
        // pod const value: can create optional with pod
        auto o1 = nel::Optional<int const>::Some(1);
        nel::unused(o1);
    }

    Stub::reset();
    {
        // udt const value: can create optional with pod
        auto o1 = nel::Optional<Stub const>::Some(Stub(1));
        CHECK(Stub::dtor == 1);

        nel::unused(o1);
        CHECK(Stub::instances == 1);
    }
    // and udt is destroyed on optional<some> destroy.
    CHECK(Stub::dtor == 1 + 1);
    CHECK(Stub::instances == 0);

    // {
    //     // void const value: ???
    //     auto o1 = nel::Optional<void>::Some();
    //     nel::unused(o1);
    // }

    {
        // pod ptr: can create optional with pod ptr
        int i1 = 1;
        auto o1 = nel::Optional<int *>::Some(&i1);
        nel::unused(o1);
    }

    Stub::reset();
    {
        // udt ptr: can create optional with pod ptr
        auto s1 = Stub(1);
        CHECK(Stub::instances == 1);
        {
            auto o1 = nel::Optional<Stub *>::Some(&s1);
            nel::unused(o1);
            // no new instance is created..
            CHECK(Stub::instances == 1);
        }
        // and udt is *not* destroyed on optional<some> destroy.
        CHECK(Stub::instances == 1);
    }

    {
        // void ptr: can create optional with void ptr
        void *p = nullptr;
        auto o1 = nel::Optional<void *>::Some(&p);
        nel::unused(o1);
    }

    {
        // pod const ptr: can create optional with pod ptr
        int const i1 = 1;
        auto o1 = nel::Optional<int const *>::Some(&i1);
        nel::unused(o1);
    }

    Stub::reset();
    {
        // udt const ptr: can create optional with pod ptr
        auto const s1 = Stub(1);
        CHECK(Stub::instances == 1);
        {
            auto o1 = nel::Optional<Stub const *>::Some(&s1);
            nel::unused(o1);
            // no new udt instances created.
            CHECK(Stub::instances == 1);
        }
        // and udt is *not* destroyed on optional<some> destroy.
        CHECK(Stub::instances == 1);
    }

    {
        // void const ptr: can create optional with void ptr
        void const *p = nullptr;
        auto o1 = nel::Optional<void const *>::Some(&p);
        nel::unused(o1);
    }

    {
        // pod ref: can create optional with pod ref
        int i1 = 1;
        int &i2 = i1;
        auto o1 = nel::Optional<int &>::Some(i2);

        // and can mutate the original via the ref in the optional...
        o1.unwrap() = 23;
        CHECK(i1 == 23);
    }

    Stub::reset();
    {
        // udt ref: can create optional with pod ptr
        auto s1 = Stub(1);
        CHECK(Stub::instances == 1);
        {
            auto &s2 = s1;
            auto o1 = nel::Optional<Stub &>::Some(s2);

            // and no new instances created
            CHECK(Stub::instances == 1);
        }
        // and udt is *not* destroyed on optional<some> destroy.
        CHECK(Stub::instances == 1);

        {
            CHECK(Stub::instances == 1);
            auto &s2 = s1;
            auto o1 = nel::Optional<Stub &>::Some(s2);

            // and can mutate the original via the ref in the optional...
            o1.unwrap().val = 23;
            // note: o1 must now be invalid (by unwrap) so would have called the dtor of contained
            // type here..
            CHECK(s1.val == 23);
        }
        CHECK(Stub::instances == 1);
    }

    {
        // pod ref: can create optional with pod ref
        int const i1 = 1;
        int const &i2 = i1;
        auto o1 = nel::Optional<int const &>::Some(i2);

        // and cannot mutate the original via the ref in the optional...
        // o1.unwrap() = 23;
        CHECK(i1 == 1);
    }

    Stub::reset();
    {
        // udt const ref: can create optional with pod ptr
        auto const s1 = Stub(1);
        CHECK(Stub::instances == 1);
        {
            auto const &s2 = s1;
            auto o1 = nel::Optional<Stub const &>::Some(s2);

            // and no new instance was created.
            CHECK(Stub::instances == 1);
        }
        // and instance referenced is not destroyed.
        CHECK(Stub::instances == 1);

        {
            auto const &s2 = s1;
            auto o1 = nel::Optional<Stub const &>::Some(s2);

            // and no new instance was created.
            CHECK(Stub::instances == 1);

            // and cannot mutate the original via the ref in the optional...
            // o1.unwrap().val = 23;
        }
    }
}

TEST_CASE("Optional::Some()::move", "[optional]")
{
    {
        // pod value: can move optional with some pod to new loc.
        auto o1 = nel::Optional<int>::Some(1);
        auto o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == 1);
    }

    {
        // pod value: can move optional with some pod to existing loc.
        auto o1 = nel::Optional<int>::Some(1);
        auto o2 = nel::Optional<int>::Some(2);

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == 1);
    }

    {
        // pod value: can move optional with some pod to existing loc.
        auto o1 = nel::Optional<int>::Some(1);
        auto o2 = nel::Optional<int>::None();

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == 1);
    }

    Stub::reset();
    {
        // udt value: can move optional with some udt to new loc
        auto o1 = nel::Optional<Stub>::Some(Stub(1));
        CHECK(Stub::instances == 1);

        auto o2 = nel::move(o1);

        // a new instance is created..
        CHECK(Stub::instances == 1 + 1);
        // udt move-ctor used.. (and used in the create)
        CHECK(Stub::move_ctor == 1 + 1);
        CHECK(Stub::move_assn == 0);
    }

    Stub::reset();
    {
        // udt value: can move optional with some udt to existing loc.
        auto o1 = nel::Optional<Stub>::Some(Stub(1));
        auto o2 = nel::Optional<Stub>::Some(Stub(2));
        CHECK(Stub::instances == 2);

        o2 = nel::move(o1);

        // no new instances created..
        // none dropped.
        CHECK(Stub::instances == 2);
        // udt move-assn used.. (and used in the 2 creates)
        CHECK(Stub::move_ctor == 2);
        CHECK(Stub::move_assn == 1);
    }

    Stub::reset();
    {
        // udt value: can move optional with some udt to existing loc.
        auto o1 = nel::Optional<Stub>::Some(Stub(1));
        auto o2 = nel::Optional<Stub>::None();
        CHECK(Stub::instances == 1);

        o2 = nel::move(o1);

        // no mew instances created..
        CHECK(Stub::instances == 1);
        // udt move-assn used.. (and move-ctor used in initial create)
        CHECK(Stub::move_ctor == 1);
        CHECK(Stub::move_assn == 1);
    }

    {
        // void value: can move optional with void to new loc.
        auto o1 = nel::Optional<void>::Some();
        auto o2 = nel::move(o1);

        // src invalidated..
        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }

    {
        // void value: can move optional with void to existing loc.
        auto o1 = nel::Optional<void>::Some();
        auto o2 = nel::Optional<void>::Some();

        o2 = nel::move(o1);

        // src invalidated..
        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }

    {
        // void value: can move optional with void  to existing loc.
        auto o1 = nel::Optional<void>::Some();
        auto o2 = nel::Optional<void>::None();

        o2 = nel::move(o1);

        // src invalidated..
        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }

    {
        // pod const value: can move optional with pod to new loc.
        auto o1 = nel::Optional<int const>::Some(1);
        auto o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == 1);
    }
    // cannot move-assn const values around..
    // {
    //     // pod const value: can move optional with pod to existing loc.
    //     auto o1 = nel::Optional<int const>::Some(1);
    //     auto o2 = nel::Optional<int const>::Some(2);

    //     o2 = nel::move(o1);

    //          CHECK(!o1.is_none());
    //          CHECK(!o1.is_some());

    //          CHECK(o2.is_some());
    //          CHECK(o2.unwrap() == 1);
    // }
    // cannot move-assn const values around..
    // {
    //     // pod const value: can move optional with pod to existing loc.
    //     auto o1 = nel::Optional<int const>::Some(1);
    //     auto o2 = nel::Optional<int const>::None();

    //     o2 = nel::move(o1);

    //          CHECK(!o1.is_none());
    //          CHECK(!o1.is_some());

    //          CHECK(o2.is_some());
    //          CHECK(o2.unwrap() == 1);
    // }

    {
        Stub::reset();
        // udt const value: can move optional with pod to new loc
        auto o1 = nel::Optional<Stub const>::Some(Stub(1));
        CHECK(Stub::instances == 1);

        auto o2 = nel::move(o1);

        // a new instance is created..
        CHECK(Stub::instances == 2);
        // move-ctor not used..
        CHECK(Stub::move_ctor == 0);
        CHECK(Stub::move_assn == 0);
        CHECK(Stub::copy_ctor == 1+1); //  cannot move const, but can copy.?
        CHECK(Stub::copy_assn == 0);
    }

    // cannot move-assn const values around..
    // {
    //     Stub::reset();
    //     // udt const value: can move optional with pod to existing loc.
    //     auto o1 = nel::Optional<Stub const>::Some(Stub(1));
    //     auto o2 = nel::Optional<Stub const>::Some(Stub(2));
    //          CHECK(Stub::instances == 2);

    //     o2 = nel::move(o1);

    //     // no new instances created..
    //          CHECK(Stub::instances == 2);
    //     // move-assn used..
    //          CHECK(Stub::move_ctor == 0);
    //          CHECK(Stub::move_assn == 1);
    //     // freed one..
    //          CHECK(Stub::dtor == 1);
    // }

    // cannot move-assn const values around..
    // {
    //     Stub::reset();
    //     // udt const value: can move optional with pod to existing loc.
    //     auto o1 = nel::Optional<Stub const>::Some(Stub(1));
    //     auto o2 = nel::Optional<Stub const>::None();
    //          CHECK(Stub::instances == 1);

    //     o2 = nel::move(o1);

    //     // no new instances created..
    //          CHECK(Stub::instances == 1);
    //     // move-assn used..
    //          CHECK(Stub::move_ctor == 0);
    //          CHECK(Stub::move_assn == 1);
    // }

    // {
    //     // void const value: ???
    //     auto o1 = nel::Optional<void>::Some();
    //     nel::unused(o1);
    // }

    {
        // pod ptr: can move optional with pod ptr to new loc.
        int i1 = 1;
        auto o1 = nel::Optional<int *>::Some(&i1);
        auto o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == &i1);
    }

    {
        // pod ptr: can move optional with pod ptr to existing loc
        int i1 = 1, i2 = 2;
        auto o1 = nel::Optional<int *>::Some(&i1);
        auto o2 = nel::Optional<int *>::Some(&i2);

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == &i1);
    }

    {
        // pod ptr: can move optional with pod ptr to existing loc.
        int i1 = 1;
        auto o1 = nel::Optional<int *>::Some(&i1);
        auto o2 = nel::Optional<int *>::None();

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == &i1);
    }

    Stub::reset();
    {
        // udt ptr: can move optional with pod ptr to new loc.
        auto s1 = Stub(1);
        CHECK(Stub::instances == 1);
        {
            auto o1 = nel::Optional<Stub *>::Some(&s1);
            auto o2 = nel::move(o1);

            // no new instance is created..
            CHECK(Stub::instances == 1);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
            CHECK(o2.unwrap() == &s1);
        }
        // and udt is *not* destroyed on optional<some> destroy.
        CHECK(Stub::dtor == 0);
        CHECK(Stub::instances == 1);

        auto s2 = Stub(2);
        // udt ptr: can move optional with pod ptr to existing loc.
        {
            CHECK(Stub::instances == 2);

            auto o1 = nel::Optional<Stub *>::Some(&s1);
            auto o2 = nel::Optional<Stub *>::Some(&s2);

            o2 = nel::move(o1);

            // no new instance is created..
            // no instances is destroyed.
            CHECK(Stub::instances == 2);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
            CHECK(o2.unwrap() == &s1);
        }

        // udt ptr: can move optional with pod ptr to existing loc.
        {
            CHECK(Stub::instances == 2);

            auto o1 = nel::Optional<Stub *>::Some(&s1);
            auto o2 = nel::Optional<Stub *>::None();

            o2 = nel::move(o1);

            // no new instance is created..
            // no instances is destroyed.
            CHECK(Stub::instances == 2);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
            CHECK(o2.unwrap() == &s1);
        }
    }

    {
        // void ptr: can move optional with void ptr to new loc
        void *p = nullptr;
        auto o1 = nel::Optional<void *>::Some(&p);
        auto o2 = nel::move(o1);
        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }
    {
        // void ptr: can move optional with void ptr to existing loc
        void *p1 = nullptr, *p2 = reinterpret_cast<void *>(0x1000);
        auto o1 = nel::Optional<void *>::Some(&p1);
        auto o2 = nel::Optional<void *>::Some(&p2);
        o2 = nel::move(o1);
        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }
    {
        // void ptr: can move optional with void ptr to existing loc
        void *p1 = nullptr;
        auto o1 = nel::Optional<void *>::Some(&p1);
        auto o2 = nel::Optional<void *>::None();
        o2 = nel::move(o1);
        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }

    {
        // pod const ptr: can move optional with pod ptr to new loc.
        int const i1 = 1;
        auto o1 = nel::Optional<int const *>::Some(&i1);
        auto o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == &i1);
    }
    {
        // pod const ptr: can move optional with pod ptr to existing loc.
        int const i1 = 1, i2 = 2;
        auto o1 = nel::Optional<int const *>::Some(&i1);
        auto o2 = nel::Optional<int const *>::Some(&i2);

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == &i1);
    }
    {
        // pod const ptr: can move optional with pod ptr to existing loc.
        int const i1 = 1;
        auto o1 = nel::Optional<int const *>::Some(&i1);
        auto o2 = nel::Optional<int const *>::None();

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        CHECK(o2.unwrap() == &i1);
    }

    Stub::reset();
    {
        // udt const ptr: can create optional with pod ptr
        auto const s1 = Stub(1);
        CHECK(Stub::instances == 1);
        {
            auto o1 = nel::Optional<Stub const *>::Some(&s1);
            auto o2 = nel::move(o1);

            // no new udt instances created.
            CHECK(Stub::instances == 1);
            // no stubs moved..
            CHECK(Stub::move_ctor == 0);
            CHECK(Stub::move_assn == 0);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }
        // and udt is *not* destroyed on optional<some> destroy.
        CHECK(Stub::dtor == 0);
        CHECK(Stub::instances == 1);

        auto const s2 = Stub(2);
        {
            auto o1 = nel::Optional<Stub const *>::Some(&s1);
            auto o2 = nel::Optional<Stub const *>::Some(&s2);

            CHECK(Stub::instances == 2);

            o2 = nel::move(o1);

            // no new udt instances created.
            CHECK(Stub::instances == 2);
            // no stubs moved..
            CHECK(Stub::move_ctor == 0);
            CHECK(Stub::move_assn == 0);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }
        {
            auto o1 = nel::Optional<Stub const *>::Some(&s1);
            auto o2 = nel::Optional<Stub const *>::None();

            CHECK(Stub::instances == 2);
            o2 = nel::move(o1);

            // no new udt instances created.
            CHECK(Stub::instances == 2);
            // no stubs moved..
            CHECK(Stub::move_ctor == 0);
            CHECK(Stub::move_assn == 0);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }
    }

    {
        // void const ptr: can move optional with void ptr to new loc.
        void const *p = nullptr;
        auto o1 = nel::Optional<void const *>::Some(&p);
        auto o2 = nel::move(o1);
        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }
    {
        // void const ptr: can move optional with void ptr to existing.
        void const *p1 = nullptr, *p2 = reinterpret_cast<void const *>(0x1000);
        auto o1 = nel::Optional<void const *>::Some(&p1);
        auto o2 = nel::Optional<void const *>::Some(&p2);

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }
    {
        // void const ptr: can move optional with void ptr to existing
        void const *p1 = nullptr;
        auto o1 = nel::Optional<void const *>::Some(&p1);
        auto o2 = nel::Optional<void const *>::None();

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
    }

    {
        // pod ref: can move optional with pod ref to new.
        int i1 = 1;
        int &ri1 = i1;
        auto o1 = nel::Optional<int &>::Some(ri1);
        auto o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());

        // and can mutate the original via the ref in the optional...
        o2.unwrap() = 23;
        CHECK(i1 == 23);
    }

    {
        // pod ref: can move optional with pod ref to existing
        int i1 = 1, i2 = 2;
        int &ri1 = i1;
        int &ri2 = i2;
        auto o1 = nel::Optional<int &>::Some(ri1);
        auto o2 = nel::Optional<int &>::Some(ri2);

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());

        // and can mutate the original via the ref in the optional...
        o2.unwrap() = 23;
        CHECK(i1 == 23);
    }

    {
        // pod ref: can move optional with pod ref to existing.
        int i1 = 1;
        int &ri1 = i1;
        auto o1 = nel::Optional<int &>::Some(ri1);
        auto o2 = nel::Optional<int &>::None();

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());

        // and can mutate the original via the ref in the optional...
        o2.unwrap() = 23;
        CHECK(i1 == 23);
    }

    Stub::reset();
    {
        // udt ref: can move optional with pod ptr to new.
        auto s1 = Stub(1);
        CHECK(Stub::instances == 1);
        {
            auto &rs1 = s1;
            auto o1 = nel::Optional<Stub &>::Some(rs1);
            auto o2 = nel::move(o1);

            // and no new instances created
            CHECK(Stub::instances == 1);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }

        auto s2 = Stub(2);
        {
            // udt ref: can move optional with pod ptr to existing.
            auto &rs1 = s1;
            auto &rs2 = s2;
            auto o1 = nel::Optional<Stub &>::Some(rs1);
            auto o2 = nel::Optional<Stub &>::Some(rs2);

            CHECK(Stub::instances == 2);

            o2 = nel::move(o1);

            // and no new instances created
            CHECK(Stub::instances == 2);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }
        {
            // udt ref: can move optional with pod ptr to existing.
            auto &rs1 = s1;
            auto o1 = nel::Optional<Stub &>::Some(rs1);
            auto o2 = nel::Optional<Stub &>::None();

            CHECK(Stub::instances == 2);

            o2 = nel::move(o1);

            // and no new instances created
            CHECK(Stub::instances == 2);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }
    }

    {
        // pod ref: can move optional with pod ref to new.
        int const i1 = 1;
        int const &i2 = i1;
        auto o1 = nel::Optional<int const &>::Some(i2);
        auto o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        // and cannot mutate the original via the ref in the optional...
        // o1.unwrap() = 23;
        CHECK(i1 == 1);
    }
    {
        // pod ref: can move optional with pod ref to existing.
        int const i1 = 1, i2 = 2;
        int const &ri1 = i1, &ri2 = i2;
        auto o1 = nel::Optional<int const &>::Some(ri1);
        auto o2 = nel::Optional<int const &>::Some(ri2);

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        // and cannot mutate the original via the ref in the optional...
        // o1.unwrap() = 23;
        CHECK(i1 == 1);
    }
    {
        // pod ref: can move optional with pod ref to existing.
        int const i1 = 1;
        int const &ri1 = i1;
        auto o1 = nel::Optional<int const &>::Some(ri1);
        auto o2 = nel::Optional<int const &>::None();

        o2 = nel::move(o1);

        CHECK(o1.is_inval());

        CHECK(o2.is_some());
        // and cannot mutate the original via the ref in the optional...
        // o1.unwrap() = 23;
        CHECK(i1 == 1);
    }

    {
        Stub::reset();
        // udt const ref: can move optional with pod ptr to new.
        auto const s1 = Stub(1);
        auto const s2 = Stub(2);
        CHECK(Stub::instances == 2);

        {
            auto const &rs1 = s1;
            auto o1 = nel::Optional<Stub const &>::Some(rs1);
            auto o2 = nel::move(o1);

            // and no new instance was created.
            CHECK(Stub::instances == 2);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }
        // and instance referenced is not destroyed.
        CHECK(Stub::instances == 2);

        {
            // udt const ref: can move optional with pod ptr to existing.
            auto const &rs1 = s1;
            auto const &rs2 = s2;
            auto o1 = nel::Optional<Stub const &>::Some(rs1);
            auto o2 = nel::Optional<Stub const &>::Some(rs2);

            CHECK(Stub::instances == 2);

            o2 = nel::move(o1);

            // and no new instance was created.
            CHECK(Stub::instances == 2);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }

        {
            // udt const ref: can move optional with pod ptr to existing.
            auto const &rs1 = s1;
            auto o1 = nel::Optional<Stub const &>::Some(rs1);
            auto o2 = nel::Optional<Stub const &>::None();

            CHECK(Stub::instances == 2);

            o2 = nel::move(o1);

            // and no new instance was created.
            CHECK(Stub::instances == 2);

            CHECK(o1.is_inval());

            CHECK(o2.is_some());
        }
    }
}

TEST_CASE("optional(Some): unwrap for some must produce some's value", "[optional]")
{
    nel::Optional<int> opt1a = nel::Some(1);

    auto val = opt1a.unwrap();

    CHECK(val == 1);
}

TEST_CASE("optional(None): unwrap_or(2) must produce 2", "[optional]")
{
    nel::Optional<int> opt1b = nel::Optional<int>::None();

    auto val = opt1b.unwrap_or(2);

    CHECK(val == 2);
}

TEST_CASE("move(optional)-ctor for some must move val to dest", "[optional]")
{
    nel::Optional<int> opt1c = nel::Some(1);
    auto opt2 = move(opt1c);

    auto val = opt2.unwrap();

    CHECK(val == 1);
}

TEST_CASE("move(optional)-ctor for none must move val to dest", "[optional]")
{
    nel::Optional<int> opt1d = nel::Optional<int>::None();

    auto val = opt1d.unwrap_or(2);

    CHECK(val == 2);
}

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ctor for inval must move val to dest", "[optional]")
{
    nel::Optional<int> opt1e = nel::Optional<int>::None();
    auto opt2 = move(opt1e);
    auto opt3 = move(opt1e);

    CHECK(!opt3.is_some());
    CHECK(!opt3.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ctor for some must invalidate src", "[optional]")
{
    nel::Optional<int> opt1f = nel::Some(1);
    auto opt2 = move(opt1f);

    CHECK(!opt1f.is_some());
    CHECK(!opt1f.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ctor for none must invalidate src", "[optional]")
{
    nel::Optional<int> opt1g = nel::Optional<int>::None();
    auto opt2 = move(opt1g);

    CHECK(!opt1g.is_some());
    CHECK(!opt1g.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ctor for inval must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);
    auto opt3 = move(opt1);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}
#endif

TEST_CASE("move(optional)-ass for some must move val to dest", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);
    nel::Optional<int> opt2 = nel::Some(2);
    opt2 = move(opt1);

    auto val = opt2.unwrap();

    CHECK(val == 1);
}

TEST_CASE("move(optional)-ass for none must move val to dest", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    nel::Optional<int> opt2 = nel::Some(1);
    opt2 = move(opt1);

    auto val = opt2.unwrap_or(2);

    CHECK(val == 2);
}

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ass for inval must move val to dest", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    opt3 = move(opt1);

    CHECK(!opt3.is_some());
    CHECK(!opt3.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ass for some must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    nel::Optional<int> opt2 = nel::Some(2);
    opt2 = move(opt1);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ass for none must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    nel::Optional<int> opt2 = nel::Some(2);
    opt2 = move(opt1);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("move(optional)-ass for inval must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(2);
    opt3 = move(opt1);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}
#endif

TEST_CASE("Optional.is_some for some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    CHECK(opt1.is_some());
}

TEST_CASE("Optional.is_some for none must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    CHECK(!opt1.is_some());
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.is_some for inval must panic", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    CHECK(!opt1.is_some());
}
#endif

TEST_CASE("Optional.is_some for some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is = opt1.is_some();
    NEL_UNUSED(is);

    auto val = opt1.unwrap();
    CHECK(val == 1);
}

TEST_CASE("Optional.is_some for none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    auto is = opt1.is_some();
    NEL_UNUSED(is);

    CHECK(opt1.is_none());
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.is_some for inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    auto is = opt1.is_some();
    NEL_UNUSED(is);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}
#endif

TEST_CASE("Optional.is_none for some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    CHECK(!opt1.is_none());
}

TEST_CASE("Optional.is_none for none must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    CHECK(opt1.is_none());
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.is_none for inval must panic", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    CHECK(!opt1.is_none());
}
#endif

TEST_CASE("Optional.is_none for some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is = opt1.is_none();
    NEL_UNUSED(is);

    auto val = opt1.unwrap();
    CHECK(val == 1);
}

TEST_CASE("Optional.is_none for none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    auto is = opt1.is_none();
    NEL_UNUSED(is);

    CHECK(opt1.is_none());
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.is_none for inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    auto is = opt1.is_none();
    NEL_UNUSED(is);

    CHECK(!opt1.is_none());
    CHECK(!opt1.is_some());
}
#endif

TEST_CASE("Optional.unwrap for Some must give some value", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto val = opt1.unwrap();

    CHECK(val == 1);
}

#if 0
TEST_CASE("Optional.unwrap for None aborts", "[optional]")
{
// how do I do this?
}

TEST_CASE("Optional.unwrap for Inval aborts", "[optional]")
{
// how do I do this?
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap for Some must invalidate src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto val = opt1.unwrap();
    NEL_UNUSED(val);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}
#endif

TEST_CASE("Optional.unwrap with default for Some must give some value", "[optional]")
{
    nel::Optional<int> opt1a = nel::Some(1);

    auto val = opt1a.unwrap_or(2);

    CHECK(val == 1);
}

TEST_CASE("Optional.unwrap with default for None must give or value", "[optional]")
{
    nel::Optional<int> opt1b = nel::Optional<int>::None();

    auto val = opt1b.unwrap_or(2);

    CHECK(val == 2);
}

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap with default for Inval must give or value", "[optional]")
{
    nel::Optional<int> opt1c = nel::Optional<int>::None();
    auto opt2 = move(opt1c);

    auto val = opt1c.unwrap_or(2);

    CHECK(val == 2);
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap with default for Some must invalidate src", "[optional]")
{
    nel::Optional<int> opt1d = nel::Some(1);

    auto val = opt1d.unwrap_or(2);
    NEL_UNUSED(val);

    CHECK(!opt1d.is_some());
    CHECK(!opt1d.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap with default for None must invalidate src", "[optional]")
{
    nel::Optional<int> opt1e = nel::Optional<int>::None();

    auto val = opt1e.unwrap_or(2);
    NEL_UNUSED(val);

    CHECK(!opt1e.is_some());
    CHECK(!opt1e.is_none());
}
#endif

#if 0
// disabled until panic testing is available
TEST_CASE("Optional.unwrap with default for Inval must invalidate src", "[optional]")
{
    nel::Optional<int> opt1f = nel::Optional<int>::None();
    auto opt2 = move(opt1f);

    auto val = opt1f.unwrap_or(2);
    NEL_UNUSED(val);

    CHECK(!opt1f.is_some());
    CHECK(!opt1f.is_none());
}
#endif

#if 0
TEST_CASE("Optional.eq for some with some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    CHECK(opt1 == nel::Some(1));
}


TEST_CASE("Optional.eq for some with diff some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    CHECK(!(opt1 == nel::Some(2)));
}


TEST_CASE("Optional.eq for some with none must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    CHECK(!(opt1 == None));
}


TEST_CASE("Optional.eq for some with inval must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    CHECK(!(opt1 == opt2));
}


TEST_CASE("Optional.eq for some with some must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_eq = opt1 == nel::Some(1);
    NEL_UNUSED(is_eq);

    CHECK(opt1.unwrap() == 1);
}


TEST_CASE("Optional.eq for some with diff some must not change src",
          "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_eq = opt1 == nel::Some(2);
    NEL_UNUSED(is_eq);

    CHECK(opt1.unwrap() == 1);
}


TEST_CASE("Optional.eq for some with none must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    nel::Optional<int> is_eq = opt1 == nel::None;
    NEL_UNUSED(is_eq);

    CHECK(opt1.unwrap() == 1);
}


TEST_CASE("Optional.eq for some with inval must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    auto is_eq = opt1 == opt2;
    NEL_UNUSED(is_eq);

    CHECK(opt1.unwrap() == 1);
}


TEST_CASE("Optional.eq for none with some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    CHECK(!(opt1 == nel::Some(1)));
}


TEST_CASE("Optional.eq for none with none must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    CHECK(opt1 == None);
}


TEST_CASE("Optional.eq for none with inval must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    auto opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    CHECK(!(opt1 == opt2));
}



TEST_CASE("Optional.eq for none with some must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    nel::Optional<int> is_eq = opt1 == nel::Some(1);
    NEL_UNUSED(is_eq);

    CHECK(opt1.is_none());
}


TEST_CASE("Optional.eq for none with none must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    nel::Optional<int> is_eq = opt1 == nel::Optional<int>::None();
    NEL_UNUSED(is_eq);

    CHECK(opt1.is_none());
}


TEST_CASE("Optional.eq for none with inval must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    nel::Optional<int> opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    auto is_eq = opt1 == opt2;
    NEL_UNUSED(is_eq);

    CHECK(opt1.is_none());
}



TEST_CASE("Optional.eq for inval with some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    CHECK(!(opt1 == nel::Some(1)));
}



TEST_CASE("Optional.eq for inval with none must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    CHECK(!(opt1 == None));
}


TEST_CASE("Optional.eq for inval with inval must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    auto opt4 = move(opt3);

    CHECK(opt1 == opt3);
}



TEST_CASE("Optional.eq for inval with some must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> is_eq = opt1 == nel::Some(1);
    NEL_UNUSED(is_eq);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}



TEST_CASE("Optional.eq for inval with none must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> is_eq = opt1 == nel::Optional<int>::None();
    NEL_UNUSED(is_eq);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}


TEST_CASE("Optional.eq for inval with inval must not change src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    auto opt4 = move(opt3);

    auto is_eq = opt1 == opt3;
    NEL_UNUSED(is_eq);

    CHECK(!opt1.is_some());
    CHECK(!opt1.is_none());
}



TEST_CASE("Optional.neq for some with some must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    CHECK(!(opt1 != nel::Some(1)));
}


TEST_CASE("Optional.neq for some with diff some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    CHECK(opt1 != nel::Some(2));
}


TEST_CASE("Optional.neq for some with none must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    CHECK(opt1 != None);
}


TEST_CASE("Optional.neq for some with inval must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    nel::Optional<int> opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    CHECK(opt1 != opt2);
}


TEST_CASE("Optional.neq for some with some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_neq = opt1 != nel::Some(1);
    NEL_UNUSED(is_neq);

    CHECK(opt1.unwrap() == 1);
}


TEST_CASE("Optional.neq for some with diff must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_neq = opt1 != nel::Some(2);
    NEL_UNUSED(is_neq);

    CHECK(opt1.unwrap() == 1);
}


TEST_CASE("Optional.neq for some with none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    auto is_neq = opt1 != nel::None;
    NEL_UNUSED(is_neq);

    CHECK(opt1.unwrap() == 1);
}


TEST_CASE("Optional.neq for some with inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Some(1);

    nel::Optional<int> opt2 = nel::Some(2);
    auto opt3 = move(opt2);

    auto is_neq = opt1 != opt2;
    NEL_UNUSED(is_neq);

    CHECK(opt1.unwrap() == 1);
}



TEST_CASE("Optional.neq for none with some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    CHECK(opt1 != nel::Some(1));
}


TEST_CASE("Optional.neq for none with none must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    CHECK(!(opt1 != None));
}


TEST_CASE("Optional.neq for none with inval must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    nel::Optional<int> opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    CHECK(opt1 != opt2);
}


TEST_CASE("Optional.neq for none with some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    auto is_neq = opt1 != nel::Some(1);
    NEL_UNUSED(is_neq);

    CHECK(opt1.is_none());
}


TEST_CASE("Optional.neq for none with none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    auto is_neq = opt1 != nel::None;
    NEL_UNUSED(is_neq);

    CHECK(opt1.is_none());
}


TEST_CASE("Optional.neq for none with inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();

    nel::Optional<int> opt2 = nel::Some(1);
    auto opt3 = move(opt2);

    auto is_neq = opt1 != opt2;
    NEL_UNUSED(is_neq);

    CHECK(opt1.is_none());
}


TEST_CASE("Optional.neq for inval with some must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    CHECK(opt1 != nel::Some(1));
}


TEST_CASE("Optional.neq for inval with none must give true", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    CHECK(opt1 != None);
}


TEST_CASE("Optional.neq for inval with inval must give false", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    auto opt4 = move(opt3);

    CHECK(!(opt1 != opt3));
}


TEST_CASE("Optional.neq for inval with some must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    auto is_neq = opt1 != nel::Some(1);
    NEL_UNUSED(is_neq);

    CHECK(!opt1.is_none());
    CHECK(!opt1.is_some());
}


TEST_CASE("Optional.neq for inval with none must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> is_neq = opt1 != nel::Optional<int>::None();
    NEL_UNUSED(is_neq);

    CHECK(!opt1.is_none());
    CHECK(!opt1.is_some());
}


TEST_CASE("Optional.neq for inval with inval must not alter src", "[optional]")
{
    nel::Optional<int> opt1 = nel::Optional<int>::None();
    auto opt2 = move(opt1);

    nel::Optional<int> opt3 = nel::Some(1);
    auto opt4 = move(opt3);

    auto is_neq = opt1 != opt3;
    NEL_UNUSED(is_neq);

    CHECK(!opt1.is_none());
    CHECK(!opt1.is_some());
}
#endif

nel::Optional<int> foo_some()
{
    return nel::Some(1);
    // return nel::Some(1);
}

TEST_CASE("optional::Some auto into Optional", "[optional]")
{
    CHECK(foo_some().is_some());
}

nel::Optional<int> foo_none()
{
    return nel::Optional<int>::None();
}

TEST_CASE("optional::None auto into Optional", "[optional]")
{
    CHECK(foo_none().is_none());
}

struct Foo
{
    public:
        ~Foo(void)
        {
            this->dtor_called = true;
        }

        Foo(bool &dtor_called)
            : dtor_called(dtor_called)
        {
        }

    private:
        bool &dtor_called;
};

TEST_CASE("optional::~dtor for some, must call some dtor", "[optional]")
{
    bool dtor_called = false;
    {
        nel::Optional<Foo> opt1 = nel::Some(Foo(dtor_called));

        dtor_called = false;
    }
    CHECK(dtor_called);
}

TEST_CASE("optional::map for some must produce a some", "[optional]")
{
    nel::Optional<int> opt1a = nel::Some(1);
    auto opt2a = opt1a.map<char const *>([](auto &&) -> char const * { return "haha"; });
    auto val = opt2a.unwrap();

    CHECK(strcmp(val, "haha") == 0);
}

TEST_CASE("optional::map for none must produce a none", "[optional]")
{
    nel::Optional<int> opt1a = nel::Optional<int>::None();
    auto opt2a = opt1a.map<char const *>([](auto &&) -> char const * { return "haha"; });
    CHECK(opt2a.is_none());
}

}; // namespace optional
}; // namespace test
}; // namespace nel
