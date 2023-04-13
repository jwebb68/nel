// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/memory.hh>

#include <catch2/catch.hpp>

#include <nel/stub.hh>

#include <cstring> // memset/memcmp

namespace nel
{
namespace test
{
namespace elem
{

TEST_CASE("elem::copy", "[elem]")
{
    {
        // udt can copy mut array.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(0), Stub(0), Stub(0)};

        Stub::reset();

        // copy expects the dest to already be initialised.
        nel::elem::copy(a2, a1, 3);

        // no new values exist
        CHECK(Stub::instances == 0);
        // copy-assgn was called.
        CHECK(Stub::copy_assn == 3);
        // copy-ctor was not;
        CHECK(Stub::copy_ctor == 0);

        // src is still valid.
        CHECK(a1[0].valid);
        CHECK(a1[0].val == 1);
        CHECK(a1[1].valid);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].valid);
        CHECK(a1[2].val == 3);

        // dest has new values, and is valid.
        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);
        CHECK(a2[0].val == 1);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }

    {
        // udt can copy const array.
        Stub const a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(0), Stub(0), Stub(0)};

        Stub::reset();

        // copy expects the dest to already be initialised.
        nel::elem::copy(a2, a1, 3);

        // no new values exist
        CHECK(Stub::instances == 0);
        // copy-assgn was called.
        CHECK(Stub::copy_assn == 3);
        // copy-ctor was not;
        CHECK(Stub::copy_ctor == 0);

        // src is still valid.
        CHECK(a1[0].valid);
        CHECK(a1[0].val == 1);
        CHECK(a1[1].valid);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].valid);
        CHECK(a1[2].val == 3);

        // dest has new values, and is valid.
        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);
        CHECK(a2[0].val == 1);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }

    {
        // udt can copy const array.
        Stub const a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(0), Stub(0), Stub(0)};

        Stub::reset();

        // copy expects the dest to already be initialised.
        nel::elem::copy(a2, a1, 2);

        // no new values exist
        CHECK(Stub::instances == 0);
        // copy-assgn was called.
        CHECK(Stub::copy_assn == 2);
        // copy-ctor was not;
        CHECK(Stub::copy_ctor == 0);

        // src is still valid.
        CHECK(a1[0].valid);
        CHECK(a1[0].val == 1);
        CHECK(a1[1].valid);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].valid);
        CHECK(a1[2].val == 3);

        // dest has new values, and is valid.
        CHECK(a2[0].valid);
        CHECK(a2[0].val == 1);
        CHECK(a2[1].valid);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].valid);
        CHECK(a2[2].val == 0);
    }

    {
        // can copy onto self, does nothing.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};

        Stub::reset();

        // copy expects the dest to already be initialised.
        nel::elem::copy(a1, a1, 2);

        // no new values exist
        CHECK(Stub::instances == 0);
        // copy-assgn was not called.
        CHECK(Stub::copy_assn == 0);
        // copy-ctor was not;
        CHECK(Stub::copy_ctor == 0);

        // src is still valid.
        CHECK(a1[0].valid);
        CHECK(a1[0].val == 1);
        CHECK(a1[1].valid);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].valid);
        CHECK(a1[2].val == 3);
    }

    {
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[3];
        nel::elem::copy(a2, a1, 3);
        CHECK(a2[0] == 1);
        CHECK(a2[1] == 2);
        CHECK(a2[2] == 3);
    }

    {
        int a1[] {1, 2, 3};
        int a2[3];
        nel::elem::copy(a2, a1, 3);
        CHECK(a2[0] == 1);
        CHECK(a2[1] == 2);
        CHECK(a2[2] == 3);
    }
}

TEST_CASE("elem::set", "[elem]")
{
    {
        // udt can set into mut array.
        Stub a1[] {Stub(0), Stub(0), Stub(0)};

        // set expects the dest to already be initialised.
        auto t = Stub(4);

        Stub::reset();
        nel::elem::set(a1, t, 3);

        // no new instances created.
        CHECK(Stub::instances == 0);
        // copy-assgn was called for list
        CHECK(Stub::copy_assn == 3);
        // copy-ctor was not called;
        CHECK(Stub::copy_ctor == 0);

        // template is unchanged.
        CHECK(t.valid);
        CHECK(t.val == 4);

        // dst is valid.
        CHECK(a1[0].valid);
        CHECK(a1[0].val == 4);
        CHECK(a1[1].valid);
        CHECK(a1[1].val == 4);
        CHECK(a1[2].valid);
        CHECK(a1[2].val == 4);
    }
    // {
    //     // udt cannot set into const array.
    //     Stub const a1[] { Stub(0), Stub(0), Stub(0) };

    //     Stub::reset();

    //     // set expects the dest to already be initialised.
    //     auto t = Stub(4);
    //     nel::elem::set(a1, t, 3);
    // }

    {
        uint8_t a1[3];
        nel::elem::set(a1, 2, 3);
        CHECK(a1[0] == 2);
        CHECK(a1[1] == 2);
        CHECK(a1[2] == 2);
    }

    {
        int a1[3];
        nel::elem::set(a1, 2, 3);
        CHECK(a1[0] == 2);
        CHECK(a1[1] == 2);
        CHECK(a1[2] == 2);
    }
}

TEST_CASE("elem::wipe", "[elem]")
{
    {
        // wipe single value
        auto v = Stub(1);

        Stub::reset();
        nel::elem::wipe(v);

        // no new instances created
        CHECK(Stub::instances == 0);

        uint8_t c[sizeof(Stub)];
        std::memset(c, 0xa5, sizeof(c));

        CHECK(std::memcmp(&v, c, sizeof(c)) == 0);
    }

    {
        // wipe array: ptr+len
        Stub v[] = {Stub(1), Stub(2), Stub(3)};
        nel::elem::wipe(v, 3);

        uint8_t c[sizeof(Stub) * 3];
        std::memset(c, 0xa5, sizeof(c));

        CHECK(std::memcmp(v, c, sizeof(c)) == 0);
    }

    {
        // wipe array: b+e
        Stub v[] = {Stub(1), Stub(2), Stub(3)};
        nel::elem::wipe(v, v + 3);

        uint8_t c[sizeof(Stub) * 3];
        std::memset(c, 0xa5, sizeof(c));

        CHECK(std::memcmp(v, c, sizeof(c)) == 0);
    }
}

TEST_CASE("elem::move", "[elem]")
{
    {
        // udt can move mut array.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(0), Stub(0), Stub(0)};

        Stub::reset();

        // move expects the dest to already be initialised.
        nel::elem::move(a2, a1, 3);

        // no new values exist
        CHECK(Stub::instances == 0);
        // move-assgn was called.
        CHECK(Stub::move_assn == 3);
        // move-ctor was not;
        CHECK(Stub::move_ctor == 0);

        // src is invalid.
        CHECK(!a1[0].valid);
        CHECK(!a1[1].valid);
        CHECK(!a1[2].valid);

        // dest has new values, and is valid.
        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);
        CHECK(a2[0].val == 1);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }

    // {
    //     // udt cannot move const array.
    //     Stub const a1[] { Stub(1), Stub(2), Stub(3) };
    //     Stub a2[] { Stub(0), Stub(0), Stub(0) };

    //     Stub::reset();

    //     nel::elem::move(a2, a1, 3);
    // }

    {
        // can move onto self, does nothing.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};

        Stub::reset();

        // copy expects the dest to already be initialised.
        nel::elem::move(a1, a1, 2);

        // no new values exist
        CHECK(Stub::instances == 0);
        // move-assgn was not called.
        CHECK(Stub::move_assn == 0);
        // move-ctor was not;
        CHECK(Stub::move_ctor == 0);

        // src is still valid.
        CHECK(a1[0].valid);
        CHECK(a1[0].val == 1);
        CHECK(a1[1].valid);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].valid);
        CHECK(a1[2].val == 3);
    }

    {
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[3];
        nel::elem::move(a2, a1, 3);
        CHECK(a2[0] == 1);
        CHECK(a2[1] == 2);
        CHECK(a2[2] == 3);
    }

    {
        int a1[] {1, 2, 3};
        int a2[3];
        nel::elem::move(a2, a1, 3);
        CHECK(a2[0] == 1);
        CHECK(a2[1] == 2);
        CHECK(a2[2] == 3);
    }
}

TEST_CASE("elem::eq", "[elem]")
{
    {
        // mut pod: comparison of self over 0-len region is eq.
        // empty regions are always eq.
        int a1[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, a1, 0));
    }
    {
        // mut pod: comparison of two eq 0-len regions is eq.
        // empty regions are always equal.
        int a1[] {1, 2, 3};
        int a2[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, a2, 0));
    }
    {
        // mut pod: comparison of diff 0-len regions is true.
        // empty regions are always equal.
        int a1[] {1, 2, 3};
        int a2[] {10, 21, 233};
        CHECK(nel::elem::eq(a1, a2, 0));
    }
    {
        // mut pod: comparison of nullptr over 0-len region is true.
        // empty regions are always equal.
        CHECK(nel::elem::eq(nullptr, nullptr, 0));
    }
    {
        // mut pod: comparison of region to nullptr 0-len regions is true.
        // empty regions are always equal.
        int a1[] {1, 2, 3};
        CHECK(nel::elem::eq(nullptr, a1, 0));
    }
    {
        // comparison of region to nullptr 0-len regions is true.
        // empty regions are always equal.
        int a1[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, nullptr, 0));
    }
    // {
    //     //comparison of region to nullptr non-0-len regions is undefined.
    //     int a1[] { 1,2,3};
    //     CHECK(!nel::elem::eq(a1, nullptr, 0));
    // }

    {
        // mut pod: comparison of self over non0-len region is true.
        // comparison of same is always eq.
        int a1[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, a1, 3));
    }
    {
        // mut pod: two regions are equal if every location has same value
        int a1[] {1, 2, 3};
        int a2[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, a2, 3));
    }
    {
        // mut pod: compare two non-eq ranges for non0 len is n-eq
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {10, 21, 233};
        CHECK(!nel::elem::eq(a1, a2, 3));
    }

    {
        // mut udt: compare of self for 0 range is eq
        // empty regions are always eq.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        CHECK(nel::elem::eq(a1, a1, 0));
    }
    {
        // mut udt: compare of 2 same regions for 0 range is eq
        // empty regions are always eq.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(1), Stub(2), Stub(3)};
        CHECK(nel::elem::eq(a1, a2, 0));
    }
    {
        // mut udt: compare of 2 diff regions for 0 range is eq
        // empty regions are always eq.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(11), Stub(21), Stub(233)};
        CHECK(nel::elem::eq(a1, a2, 0));
    }
    {
        // mut udt: compare of self for non0 range is eq
        // but may not do a member-wise compare.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        CHECK(nel::elem::eq(a1, a1, 3));
    }
    {
        // mut udt: two regions are equal if every location has same value
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(1), Stub(2), Stub(3)};
        CHECK(nel::elem::eq(a1, a2, 3));

        // elements remain valid
        CHECK(a1[0].valid);
        CHECK(a1[1].valid);
        CHECK(a1[2].valid);

        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);

        // and do not change value
        CHECK(a1[0].val == 1);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].val == 3);
        CHECK(a2[0].val == 1);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }
    {
        // mut udt: two regions are not equal if any location has diff value
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(10), Stub(2), Stub(3)};

        CHECK(!nel::elem::eq(a1, a2, 3));

        // elements remain valid
        CHECK(a1[0].valid);
        CHECK(a1[1].valid);
        CHECK(a1[2].valid);

        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);

        // and do not change value
        CHECK(a1[0].val == 1);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].val == 3);
        CHECK(a2[0].val == 10);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }

    {
        // const pod: two regions are equal if every location has same value
        int const a1[] {1, 2, 3, 4, 5};
        int const a2[] {1, 2, 3, 4, 5};

        CHECK(nel::elem::eq(a1, a2, 5));
    }

    {
        // const udt: two regions are equal if every location has same value
        Stub const a1[] {Stub(1), Stub(2), Stub(3)};
        Stub const a2[] {Stub(1), Stub(2), Stub(3)};

        CHECK(nel::elem::eq(a1, a2, 3));

        // elements remain valid
        CHECK(a1[0].valid);
        CHECK(a1[1].valid);
        CHECK(a1[2].valid);

        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);

        // and do not change value
        CHECK(a1[0].val == 1);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].val == 3);
        CHECK(a2[0].val == 1);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }

    // TODO: what of regions of diff types for which one of the types has a operator== for the
    // other? i.e. bool Foo::operator==(Bar const &) const ?

    // and check that uint8_t ne has same behaviour..
    // (it's an override, a sep impl, so needs checking separately).
    {
        // uint8: compare self for 0 range is eq
        // empty regions are always eq.
        uint8_t a1[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, a1, 0));
    }
    {
        // uint8: compare two ranges for 0 range is eq
        // empty regions are always eq.
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, a2, 0));
    }
    {
        // uint8: compare two non-eq ranges for 0 range is eq
        // empty regions are always eq.
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {10, 21, 233};
        CHECK(nel::elem::eq(a1, a2, 0));
    }

    {
        // uint8: compare self for non0 range is not-eq
        uint8_t a1[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, a1, 3));
    }
    {
        // uint8: compare two eq ranges for non0 range is eq
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {1, 2, 3};
        CHECK(nel::elem::eq(a1, a2, 3));
    }
    {
        // uint8: compare two non-eq ranges for non0 range is n-eq
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {10, 21, 233};
        CHECK(!nel::elem::eq(a1, a2, 3));
    }
}

TEST_CASE("elem::ne", "[elem]")
{
    {
        // mut pod: comparison of self over 0-len region is eq.
        // empty regions are always eq.
        int a1[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, a1, 0));
    }
    {
        // mut pod: comparison of two eq 0-len regions is eq.
        // empty regions are always equal.
        int a1[] {1, 2, 3};
        int a2[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, a2, 0));
    }
    {
        // mut pod: comparison of diff 0-len regions is eq.
        // empty regions are always equal.
        int a1[] {1, 2, 3};
        int a2[] {10, 21, 233};
        CHECK(!nel::elem::ne(a1, a2, 0));
    }
    {
        // mut pod: comparison of nullptr over 0-len region is true.
        // empty regions are always equal.
        CHECK(!nel::elem::ne(nullptr, nullptr, 0));
    }
    {
        // mut pod: comparison of region to nullptr 0-len regions is true.
        // empty regions are always equal.
        int a1[] {1, 2, 3};
        CHECK(!nel::elem::ne(nullptr, a1, 0));
    }
    {
        // mut pod: comparison of region to nullptr 0-len regions is true.
        // empty regions are always equal.
        int a1[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, nullptr, 0));
    }
    // {
    //     //comparison of region to nullptr non-0-len regions is undefined.
    //     int a1[] { 1,2,3};
    //     CHECK(!nel::elem::eq(a1, nullptr, 0));
    // }

    {
        // mut pod: comparison of self over non0-len region is eq.
        // comparison of same is always eq.
        int a1[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, a1, 3));
    }
    {
        // mut pod: two regions are equal if every location has same value
        int a1[] {1, 2, 3};
        int a2[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, a2, 3));
    }
    {
        // mut pod: two regions are not equal if any location has different value
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {10, 21, 233};
        CHECK(nel::elem::ne(a1, a2, 3));
    }

    {
        // mut udt: compare of self for 0 range is eq
        // empty regions are always eq.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        CHECK(!nel::elem::ne(a1, a1, 0));
    }
    {
        // mut udt: compare of 2 same regions for 0 range is eq
        // empty regions are always eq.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(1), Stub(2), Stub(3)};
        CHECK(!nel::elem::ne(a1, a2, 0));
    }
    {
        // mut udt: compare of 2 diff regions for 0 range is eq
        // empty regions are always eq.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(11), Stub(21), Stub(233)};
        CHECK(!nel::elem::ne(a1, a2, 0));
    }
    {
        // mut udt: compare of self for non0 range is eq
        // but may not do a member-wise compare.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        CHECK(!nel::elem::ne(a1, a1, 3));
    }
    {
        // mut udt: two regions are equal if every location has same value
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(1), Stub(2), Stub(3)};

        CHECK(!nel::elem::ne(a1, a2, 3));

        // elements remain valid
        CHECK(a1[0].valid);
        CHECK(a1[1].valid);
        CHECK(a1[2].valid);

        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);

        // and do not change value
        CHECK(a1[0].val == 1);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].val == 3);
        CHECK(a2[0].val == 1);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }
    {
        // mut udt: two regions are not equal if any location has diff value
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        Stub a2[] {Stub(10), Stub(2), Stub(3)};

        CHECK(nel::elem::ne(a1, a2, 3));

        // elements remain valid
        CHECK(a1[0].valid);
        CHECK(a1[1].valid);
        CHECK(a1[2].valid);

        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);

        // and do not change value
        CHECK(a1[0].val == 1);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].val == 3);
        CHECK(a2[0].val == 10);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }

    {
        // const pod: two regions are equal if every location has same value
        int const a1[] {1, 2, 3, 4, 5};
        int const a2[] {1, 2, 3, 4, 5};

        CHECK(!nel::elem::ne(a1, a2, 5));
    }

    {
        // const udt: two regions are equal if every location has same value
        Stub const a1[] {Stub(1), Stub(2), Stub(3)};
        Stub const a2[] {Stub(1), Stub(2), Stub(3)};

        CHECK(!nel::elem::ne(a1, a2, 3));

        // elements remain valid
        CHECK(a1[0].valid);
        CHECK(a1[1].valid);
        CHECK(a1[2].valid);

        CHECK(a2[0].valid);
        CHECK(a2[1].valid);
        CHECK(a2[2].valid);

        // and do not change value
        CHECK(a1[0].val == 1);
        CHECK(a1[1].val == 2);
        CHECK(a1[2].val == 3);
        CHECK(a2[0].val == 1);
        CHECK(a2[1].val == 2);
        CHECK(a2[2].val == 3);
    }

    // TODO: what of regions of diff types for which one of the types has a operator!= for the
    // other? i.e. bool Foo::operator!=(Bar const &) const ?

    // and check that uint8_t ne has same behaviour..
    // (it's an override, a sep impl, so needs checking separately).
    {
        // uint8: compare self for 0 range is eq
        uint8_t a1[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, a1, 0));
    }
    {
        // uint8: compare self for non0 range is not-eq
        uint8_t a1[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, a1, 3));
    }

    {
        // uint8: compare two ranges for 0 range is eq
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, a2, 0));
    }
    {
        // uint8: compare two non-eq ranges for 0 range is eq
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {10, 21, 233};
        CHECK(!nel::elem::ne(a1, a2, 0));
    }
    {
        // uint8: compare two eq ranges for non0 range is eq
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {1, 2, 3};
        CHECK(!nel::elem::ne(a1, a2, 3));
    }
    {
        // uint8: compare two non-eq ranges for non0 range is n-eq
        uint8_t a1[] {1, 2, 3};
        uint8_t a2[] {10, 21, 233};
        CHECK(nel::elem::ne(a1, a2, 3));
    }
}

}; // namespace elem
}; // namespace test
}; // namespace nel
