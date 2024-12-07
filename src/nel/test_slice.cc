// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "slice.hh"

#include <nel/stub.hh>

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace slice
{

// cannot create unset slices
// ie no auto v = Slice<T>();

TEST_CASE("Slice::destroy", "[slice]")
{
    // can have multiple slices over same data
    {
        // empty slices can be deleted.
        auto s1 = nel::Slice<Stub>::empty();
        nel::unused(s1);
    }

    {
        // as can non-empty ones.
        // slice dtor does not destroy what it points to.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        {
            auto s1 = nel::Slice(a1, 3);
            nel::unused(s1);
        }
        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
    }
}

TEST_CASE("Slice::move", "[slice]")
{
    // slices can be moved around, they do not move their data.
    {
        Stub a1[] {Stub(1), Stub(2), Stub(3)};

        auto s1 = nel::Slice(a1, 3);
        auto p1 = s1.ptr();
        auto l1 = s1.len();

        auto s2 = move(s1);

        // when moved, the src value is 'nullified'/made empty
        REQUIRE((s1.len() == 0 || s1.ptr() == nullptr));
        REQUIRE(s1.is_empty());

        REQUIRE(s2.ptr() == p1);
        REQUIRE(s2.len() == l1);

        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
    }

    {
        Stub a1[] {Stub(1), Stub(2), Stub(3)};

        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1 + 1, 2);

        auto p1 = s1.ptr();
        auto l1 = s1.len();

        s2 = nel::move(s1);

        // when moved, the src value is 'nullified'/made empty
        REQUIRE((s1.len() == 0 || s1.ptr() == nullptr));
        REQUIRE(s1.is_empty());

        REQUIRE(s2.ptr() == p1);
        REQUIRE(s2.len() == l1);

        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
    }
}

TEST_CASE("Slice::copy", "[slice]")
{
    // slices can be copied around, they do not copy their data.
    {
        Stub a1[] {Stub(1), Stub(2), Stub(3)};

        // copy existing into new.
        auto s1 = nel::Slice(a1, 3);

        auto s2 = s1;

        // on copy, the dest points to same loc+len as orig.
        REQUIRE(s2.ptr() == s1.ptr());
        REQUIRE(s2.len() == s1.len());

        // on copy, slices are the same by value.
        REQUIRE(s2 == s1);

        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
    }
    {
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        // copy exiting into existing.
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1, 2);

        s2 = s1;

        // on copy, the dest points to same loc+len as orig.
        REQUIRE(s2.ptr() == s1.ptr());
        REQUIRE(s2.len() == s1.len());

        // on copy, slices are the same by value.
        REQUIRE(s2 == s1);

        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
    }

    {
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        // copy const existing into const new
        auto const s1 = nel::Slice(a1, 3);
        auto const s2 = s1;

        // on copy, the dest points to same loc+len as orig.
        REQUIRE(s2.ptr() == s1.ptr());
        REQUIRE(s2.len() == s1.len());

        // on copy, slices are the same by value.
        REQUIRE(s2 == s1);

        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
    }
    {
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        // copy const existing into mut new.
        auto const s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1, 2);
        s2 = s1;
        // on copy, the dest points to same loc+len as orig.
        REQUIRE(s2.ptr() == s1.ptr());
        REQUIRE(s2.len() == s1.len());

        // on copy, slices are the same by value.
        REQUIRE(s2 == s1);

        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
    }
}

TEST_CASE("Slice::empty()", "[slice]")
{
    {
        auto s1 = nel::Slice<int>::empty();
        REQUIRE(s1.is_empty());
    }

    {
        auto const c1 = nel::Slice<int>::empty();
        REQUIRE(c1.is_empty());
    }
}

TEST_CASE("Slice::from(ptr,len)", "[slice]")
{
    {
        // create with mut array with mut values into mut slice
        int a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(s1.len() == 4);
    }

    {
        // create with mut array with mut values into const slice
        int a1[] = {3, 1, 2, 4};
        auto const c1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(c1.len() == 4);
    }

    {
        // create with mut array with mut values into value const slice
        int a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(s1.len() == 4);
    }

    // { // must compile fail.
    //     // create with const array with const values into value mut slice
    //     int const a1[] = {3, 1, 2, 4};
    //     auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
    //     REQUIRE(s1.len() == 4);
    // }

    {
        // create with const array with const values into value const slice
        int const a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(s1.len() == 4);
    }

    {
        // create with mut array with mut values into mut slice
        int a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(s1.len() == 4);
    }

    {
        // create with mut array with mut values into const slice
        int a1[] = {3, 1, 2, 4};
        auto const c1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(c1.len() == 4);
    }

    {
        // create with mut array with mut values into value const slice
        int a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(s1.len() == 4);
    }

    // { // must compile fail.
    //     // create with const array with const values into value mut slice
    //     int const a1[] = {3, 1, 2, 4};
    //     auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
    //     REQUIRE(s1.len() == 4);
    // }

    {
        // create with const array with const values into value const slice
        int const a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(s1.len() == 4);
    }
}

TEST_CASE("Slice::is_empty()", "[slice]")
{
    {
        auto s1 = nel::Slice<int>::empty();
        REQUIRE(s1.is_empty());
    }

    {
        auto const c1 = nel::Slice<int>::empty();
        REQUIRE(c1.is_empty());
    }

    {
        int a1[] = {3, 1, 2, 4};
        auto s2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(!s2.is_empty());

        auto const c2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(!c2.is_empty());
    }

    {
        int a1[] = {3, 1, 2, 4};
        // is empty iff len is 0..
        auto s2 = nel::Slice(a1, (nel::Length)0);
        REQUIRE(s2.is_empty());

        auto const c2 = nel::Slice(a1, (nel::Length)0);
        REQUIRE(c2.is_empty());
    }

    {
        auto s2 = nel::Slice<int>(nullptr, (nel::Length)00);
        REQUIRE(s2.is_empty());

        auto const c2 = nel::Slice<int>(nullptr, (nel::Length)0);
        REQUIRE(c2.is_empty());
    }

    {
        // emptyness is not a function of what it points to..
        auto s2 = nel::Slice<int>(nullptr, 3);
        REQUIRE(!s2.is_empty());

        auto const c2 = nel::Slice<int>(nullptr, 4);
        REQUIRE(!c2.is_empty());
    }
}

TEST_CASE("Slice::len()", "[slice]")
{
    {
        auto s1 = nel::Slice<int>::empty();
        REQUIRE(s1.len() == 0);
    }

    {
        auto const c1 = nel::Slice<int>::empty();
        REQUIRE(c1.len() == 0);
    }

    {
        int a1[] = {3, 1, 2, 4};
        auto s2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(s2.len() == 4);
    }

    {
        int a1[] = {3, 1, 2, 4};
        auto const c2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        REQUIRE(c2.len() == 4);
    }

    // note: can point to nullptr
    {
        auto s11 = nel::Slice<int>(nullptr, (nel::Length)0);
        REQUIRE(s11.len() == 0);
    }
    {
        auto s11 = nel::Slice<int>(nullptr, (nel::Length)2);
        REQUIRE(s11.len() == 2);
    }
}

TEST_CASE("Slice::fill()", "[slice]")
{
    {
        // can fill an empty slice, it just does nothing..
        auto s1 = nel::Slice<int>::empty();
        s1.fill(2);
    }
    {
        // can fill an empty slice, it just does nothing..
        auto s1 = nel::Slice<int>(nullptr, (nel::Length)0);
        s1.fill(2);
    }
    // filling a nullptr+non0 len is UB

    {
        int a1[] = {3, 1, 2, 4};
        // can fill an empty slice, it just does nothing..
        auto s1 = nel::Slice(a1, (nel::Length)0);
        s1.fill(2);
        REQUIRE(a1[0] == 3);
    }

    {
        // can fill mutable non-empty
        int a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        s1.fill(2);
        REQUIRE(a1[0] == 2);
        REQUIRE(a1[1] == 2);
        REQUIRE(a1[2] == 2);
        REQUIRE(a1[3] == 2);
    }

    // {
    //     // must not be able to fill a const slice..
    //     // must fail to compile.
    //     auto const s1 = nel::Slice<int>::empty();
    //     s1.fill(2);
    // }
    // {
    //     // must not be able to fill a const slice..
    //     // must fail to compile.
    //     auto const s1 = nel::Slice(nullptr, (nel::Length)0);
    //     s1.fill(2);
    // }
    // {
    //     int a1[] = {3, 1, 2, 4};
    //     // must not be able to fill a const slice..
    //     // must fail to compile.
    //     auto const s1 = nel::Slice(a1, (nel::Length)0);
    //     s1.fill(2);
    // }

    // and if data is const?
    // {
    //     // must not be able to fill a const slice..
    //     // must fail to compile.
    //     // fails, but with unrelated error..
    //     auto s1 = nel::Slice<int const>::empty();
    //     s1.fill(2);
    // }
    // {
    //     // must not be able to fill a const slice..
    //     // must fail to compile.
    // //     // fails, but with unrelated error..
    //     auto s1 = nel::Slice(nullptr, (nel::Length)0);
    //     s1.fill(2);
    // }
    // {
    //     int const a1[] = {3, 1, 2, 4};
    //     // must not be able to fill a const slice..
    //     // must fail to compile.
    // //     // fails, but with unrelated error..
    //     auto s1 = nel::Slice(a1, (nel::Length)0);
    //     s1.fill(2);
    // }
    // {
    //     int const a1[] = {3, 1, 2, 4};
    //     // must not be able to fill a const slice..
    //     // must fail to compile.
    // //     // fails, but with unrelated error..
    //     auto s1 = nel::Slice(a1, (nel::Length)3);
    //     s1.fill(2);
    // }
}

TEST_CASE("Slice::slice(b,e)", "[slice]")
{
    {
        // sub slice of empty vec is empty.
        auto s1 = nel::Slice<int>::empty();
        auto sa1 = s1.slice(0, 0);
        REQUIRE(sa1.is_empty());
        REQUIRE(sa1.len() == 0);

        auto sa2 = s1.slice(3, 5);
        REQUIRE(sa2.is_empty());
        REQUIRE(sa2.len() == 0);
    }

    {
        // in-range slice is not empty
        int a1[] = {3, 1};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));

        auto sa12 = s1.slice(0, 1);
        REQUIRE(!sa12.is_empty());
        REQUIRE(sa12.len() == 1);

        // out-of-range slice is empty
        auto sa13 = s1.slice(3, 4);
        REQUIRE(sa13.is_empty());

        // partially out-of-range slice is not empty, and has only up to valid items
        auto sa14 = s1.slice(1, 4);
        REQUIRE(!sa14.is_empty());
        REQUIRE(sa14.len() == 1);

        // partially out-of-range slice is not empty, and has only up to valid items
        auto sa15 = s1.slice(0, 4);
        REQUIRE(!sa15.is_empty());
        REQUIRE(sa15.len() == 2);
    }

    {
        // in-range slice is not empty
        int const a1[] = {3, 1};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));

        auto sc1 = s1.slice(0, 0);
        REQUIRE(sc1.is_empty());

        // sub slice of non-empty array is not empty.
        auto sc2 = s1.slice(0, 1);
        REQUIRE(!sc2.is_empty());
        REQUIRE(sc2.len() == 1);

        auto sc3 = s1.slice(3, 4);
        REQUIRE(sc3.is_empty());

        auto sc4 = s1.slice(1, 4);
        REQUIRE(!sc4.is_empty());
        REQUIRE(sc4.len() == 1);

        auto sc5 = s1.slice(0, 4);
        REQUIRE(!sc5.is_empty());
        REQUIRE(sc5.len() == 2);
    }
}

TEST_CASE("Slice::iter()", "[slice]")
{
#if defined(RUST_LIKE)
    {
        // udt mut slice can be iter'd
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        auto s1 = nel::Slice(a1, 3);
        auto i1 = s1.iter();
        REQUIRE(i1.next().unwrap().val == 1);
        REQUIRE(i1.next().unwrap().val == 2);
        REQUIRE(i1.next().unwrap().val == 3);
        REQUIRE(i1.next().is_none());
    }

    {
        // udt mut slice can be written through iter.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        auto s1 = nel::Slice(a1, 3);

        auto i2 = s1.iter();
        i2.next().unwrap().val += 10;
        i2.next().unwrap().val += 10;

        auto i1 = s1.iter();
        REQUIRE(i1.next().unwrap().val == 11);
        REQUIRE(i1.next().unwrap().val == 12);
        REQUIRE(i1.next().unwrap().val == 3);
        REQUIRE(i1.next().is_none());
    }
#endif
#if defined(C_LIKE)
    {
        // udt mut slice can be iter'd.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        auto s1 = nel::Slice(a1, 3);
        auto i1 = s1.iter();
        REQUIRE(i1);
        REQUIRE((*i1).val == 1);
        ++i1;
        REQUIRE(i1);
        REQUIRE((*i1).val == 2);
        ++i1;
        REQUIRE(i1);
        REQUIRE((*i1).val == 3);
        ++i1;
        REQUIRE(!i1);
    }

    {
        // udt mut slice can be written to through iter.
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        auto s1 = nel::Slice(a1, 3);

        auto i2 = s1.iter();
        (*i2).val += 10;
        ++i2;
        (*i2).val += 10;
        ++i2;

        auto i1 = s1.iter();
        REQUIRE(i1);
        REQUIRE((*i1).val == 11);
        ++i1;
        REQUIRE(i1);
        REQUIRE((*i1).val == 12);
        ++i1;
        REQUIRE(i1);
        REQUIRE((*i1).val == 3);
        ++i1;
        REQUIRE(!i1);
    }
#endif

#if defined(RUST_LIKE)
    {
        // pod empty mut slice can be iter'd
        auto s1 = nel::Slice<int>::empty();
        auto is1 = s1.iter();
        REQUIRE(is1.next().is_none());
    }

    {
        // pod empty const slice can be iter'd
        auto const c1 = nel::Slice<int>::empty();
        auto ic1 = c1.iter();
        REQUIRE(ic1.next().is_none());
    }

    {
        // pod non-empty mut slice can be iter'd
        // values are returned in order as they occur in the slice.
        int a1[] = {3, 1, 2, 4};
        auto s2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto is2 = s2.iter();

        REQUIRE(is2.next().unwrap() == 3);
        REQUIRE(is2.next().unwrap() == 1);
        REQUIRE(is2.next().unwrap() == 2);
        REQUIRE(is2.next().unwrap() == 4);
        REQUIRE(is2.next().is_none());
    }

    {
        // pod non-empty const slice can be iter'd
        // values are returned in order as they occur in the slice.
        int a1[] = {3, 1, 2, 4};
        auto const c2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto ic2 = c2.iter();

        REQUIRE(ic2.next().unwrap() == 3);
        REQUIRE(ic2.next().unwrap() == 1);
        REQUIRE(ic2.next().unwrap() == 2);
        REQUIRE(ic2.next().unwrap() == 4);
        REQUIRE(ic2.next().is_none());
    }

    {
        // pod non-empty mut slice can be written through iter.
        int a1[] = {3, 1, 2, 4};
        auto s2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto is3 = s2.iter();
        is3.next().unwrap() = 5;
        REQUIRE(a1[0] == 5);
    }

    {
        // pod non-empty const slice, cannot assign through interator
        int const a1[] = {3, 1, 2, 4};
        auto const s2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto is3 = s2.iter();
        // must compile fail.
        is3.next().unwrap() = 5;
    }

#endif

#if defined(C_LIKE)
    {
        // pod empty mut slice can be iterd
        auto s1 = nel::Slice<int>::empty();
        auto is1 = s1.iter();
        REQUIRE(!is1);
    }

    {
        // pod empty const slice can be iterd
        auto const c1 = nel::Slice<int>::empty();
        auto ic1 = c1.iter();
        REQUIRE(!ic1);
    }

    {
        // pod non-empty mut slice can be iterd
        // values are returned in order they occur in the slice.
        int a1[] = {3, 1, 2, 4};
        auto s2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto is2 = s2.iter();

        REQUIRE(is2);
        REQUIRE(*is2 == 3);
        ++is2;
        REQUIRE(is2);
        REQUIRE(*is2 == 1);
        ++is2;
        REQUIRE(is2);
        REQUIRE(*is2 == 2);
        ++is2;
        REQUIRE(is2);
        REQUIRE(*is2 == 4);
        ++is2;
        REQUIRE(!is2);
    }

    {
        // pod non-empty const slice can be iterd
        // values are returned in order they occur in the slice.
        int a1[] = {3, 1, 2, 4};
        auto const c2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto ic2 = c2.iter();

        REQUIRE(ic2);
        REQUIRE(*ic2 == 3);
        ++ic2;
        REQUIRE(ic2);
        REQUIRE(*ic2 == 1);
        ++ic2;
        REQUIRE(ic2);
        REQUIRE(*ic2 == 2);
        ++ic2;
        REQUIRE(ic2);
        REQUIRE(*ic2 == 4);
        ++ic2;
        REQUIRE(!ic2);
    }

    {
        // pod non-empty mut slice, can assign through interator
        int a1[] = {3, 1, 2, 4};
        auto s2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto is3 = s2.iter();
        *is3 = 5;

        REQUIRE(a1[0] == 5);
    }

    // {
    //     // pod non-empty const slice, cannot assign through interator
    //     int const a1[] = {3, 1, 2, 4};
    //     auto const s2 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
    //     auto is3 = s2.iter();
    //     // must compile fail.
    //     *is3 = 5;
    // }
#endif
}

TEST_CASE("Slice::try_get()", "[slice]")
{
    {
        // get on empty slice is always none.
        auto s1 = nel::Slice<int>::empty();
        auto sa1 = s1.try_get(0);
        REQUIRE(sa1.is_none());

        auto const c1 = nel::Slice<int>::empty();
        auto sc1 = c1.try_get(0);
        REQUIRE(sc1.is_none());
    }

    {
        // in-range try_get of non-empty slice is ref to value.
        int a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto sa2 = s1.try_get(0);
        REQUIRE(sa2.is_some());
        REQUIRE(sa2.unwrap() == 3);

        auto const cs1 = move(s1);
        auto scs1 = cs1.try_get(0);
        REQUIRE(scs1.is_some());
        REQUIRE(scs1.unwrap() == 3);
    }

    {
        // out-of-range try_get of non-empty slice is none.
        int a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        auto sa2 = s1.try_get(5);
        REQUIRE(sa2.is_none());

        auto const cs1 = move(s1);
        auto scs1 = cs1.try_get(5);
        REQUIRE(scs1.is_none());
    }

    {
        // can write value through get
        int a1[] = {3, 1, 2, 4};
        auto s1 = nel::Slice(a1, sizeof(a1) / sizeof(a1[0]));
        s1.try_get(0).unwrap() = 10;
        REQUIRE(a1[0] == 10);
    }

    {
        Stub::reset();
        // try_get does not copy/move the underlying data..
        Stub a1[] {Stub(1), Stub(2), Stub(3)};
        auto s1 = nel::Slice(a1, 2);

        REQUIRE(Stub::instances == 3);
        auto mc1 = Stub::move_ctor;
        auto ma1 = Stub::move_assn;
        auto cc1 = Stub::copy_ctor;
        auto ca1 = Stub::copy_assn;

        // although the try_get + optional carry the reference
        // the 'auto v1 =' converts it to a value, so invoking the copy-ctor
        // remember to always use 'auto &v1 ='.
        auto &v1 = s1.try_get(0).unwrap();

        CHECK(v1.val == 1);

        // no new instances created, none destroyed
        CHECK(Stub::instances == 3);
        // no values moved from a1.
        CHECK(Stub::move_ctor == mc1);
        CHECK(Stub::move_assn == ma1);
        // no values copied from a1.
        CHECK(Stub::copy_ctor == cc1);
        CHECK(Stub::copy_assn == ca1);
    }
}

TEST_CASE("Slice::unchecked_get()", "[slice]")
{
    {
        int a1[] = {2, 3, 4};
        auto s1 = nel::Slice(a1, 2);

        // can access inside bounds.
        REQUIRE(s1.unchecked_get(0) == 2);

        // can access outside of bounds of slice.
        REQUIRE(s1.unchecked_get(2) == 4);

        // can write-back
        s1.unchecked_get(0) = 12;
        REQUIRE(s1.unchecked_get(0) == 12);

        s1.unchecked_get(2) = 34;
        REQUIRE(s1.unchecked_get(2) == 34);
    }

    {
        int a1[] = {2, 3, 4};
        auto const c1 = nel::Slice(a1, 2);
        // can access inside bounds.
        REQUIRE(c1.unchecked_get(0) == 2);
        // can access outside of bounds of slice.
        REQUIRE(c1.unchecked_get(2) == 4);

        // cannot write-back.. how to test?
        // is it the slice that's const or it's data?
        c1.unchecked_get(0) = 12;
        REQUIRE(c1.unchecked_get(0) == 12);

        c1.unchecked_get(2) = 34;
        REQUIRE(c1.unchecked_get(2) == 34);
    }
}

// test case checked_get needs panic detection.
TEST_CASE("Slice::checked_get()", "[slice]")
{
    {
        int a1[] = {2, 3, 4};
        auto s1 = nel::Slice(a1, 2);

        // can access inside bounds.
        REQUIRE(s1.checked_get(0) == 2);

        // must not be able to access outside of bounds of slice.
        // wants panic detection.
        // REQUIRE(s1.unchecked_get(2) == 4);

        // can write-back
        s1.checked_get(0) = 12;
        REQUIRE(s1.checked_get(0) == 12);

        // must not be able to access outside of bounds of slice.
        // wants panic detection.
        // s1.checked_get(2) = 34;
        // REQUIRE(s1.checked_get(2) == 34);
    }

    {
        int a1[] = {2, 3, 4};
        auto const c1 = nel::Slice(a1, 2);
        // can access inside bounds.
        REQUIRE(c1.checked_get(0) == 2);
        // must not be able to  access outside of bounds of slice.
        // wants panic detection.
        // REQUIRE(c1.checked_get(2) == 4);

        // cannot write-back.. how to test?
        // is it the slice that's const or it's data?
        c1.checked_get(0) = 12;
        REQUIRE(c1.checked_get(0) == 12);

        // must not be able to  access outside of bounds of slice.
        // wants panic detection.
        // c1.checked_get(2) = 34;
        // REQUIRE(c1.checked_get(2) == 34);
    }
}

TEST_CASE("Slice::operator==", "[slice]")
{
    {
        // empty slices are equal
        auto s1 = nel::Slice<int>::empty();
        auto s2 = nel::Slice<int>::empty();
        REQUIRE(s1 == s2);
    }

    {
        int a1[] = {2, 3, 4};
        // empty slices are equal
        auto s1 = nel::Slice(a1, (nel::Length)0);
        auto s2 = nel::Slice(a1, (nel::Length)0);
        REQUIRE(s1 == s2);
    }

    {
        int a1[] = {2, 3, 4};
        // empty slices are equal
        auto s1 = nel::Slice(a1, (nel::Length)0);
        auto s2 = nel::Slice(a1 + 2, (nel::Length)0);
        REQUIRE(s1 == s2);
    }

    {
        int a1[] = {2, 3, 4};
        // empty slices are not equal to non-empty ones
        auto s1 = nel::Slice(a1, 2);
        auto s2 = nel::Slice(a1, (nel::Length)0);
        REQUIRE(!(s1 == s2));
    }
    {
        int a1[] = {2, 3, 4};
        // empty slices are not equal to non-empty ones
        auto s1 = nel::Slice(a1, (nel::Length)0);
        auto s2 = nel::Slice(a1, 2);
        REQUIRE(!(s1 == s2));
    }

    {
        int a1[] = {2, 3, 4};
        // slices over same data are equal
        auto s1 = nel::Slice(a1, 2);
        auto s2 = nel::Slice(a1, 2);
        REQUIRE(s1 == s2);
    }

    {
        int a1[] = {2, 3, 4};
        // slices over diff data are not.
        auto s1 = nel::Slice(a1 + 1, 2);
        auto s2 = nel::Slice(a1, 2);
        REQUIRE(!(s1 == s2));
    }
    {
        int a1[] = {2, 3, 4};
        // slices over diff data are not.
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1, 2);
        REQUIRE(!(s1 == s2));
    }

    {
        int a1[] = {2, 3, 4};
        int a2[] = {3, 4, 2};
        // slices over diff data are eq if values are same for same len
        auto s1 = nel::Slice(a1, 2);
        auto s2 = nel::Slice(a2 + 1, 2);
        REQUIRE(!(s1 == s2));
    }
}

TEST_CASE("Slice::operator!=", "[slice]")
{
    {
        // empty slices are equal
        auto s1 = nel::Slice<int>::empty();
        auto s2 = nel::Slice<int>::empty();
        REQUIRE(!(s1 != s2));
    }

    {
        int a1[] = {2, 3, 4};
        // empty slices are equal
        auto s1 = nel::Slice(a1, (nel::Length)0);
        auto s2 = nel::Slice(a1, (nel::Length)0);
        REQUIRE(!(s1 != s2));
    }

    {
        int a1[] = {2, 3, 4};
        // empty slices are equal
        auto s1 = nel::Slice(a1, (nel::Length)0);
        auto s2 = nel::Slice(a1 + 2, (nel::Length)0);
        REQUIRE(!(s1 != s2));
    }

    {
        int a1[] = {2, 3, 4};
        // empty slices are not equal to non-empty ones
        auto s1 = nel::Slice(a1, 2);
        auto s2 = nel::Slice(a1, (nel::Length)0);
        REQUIRE(s1 != s2);
    }
    {
        int a1[] = {2, 3, 4};
        // empty slices are not equal to non-empty ones
        auto s1 = nel::Slice(a1, (nel::Length)0);
        auto s2 = nel::Slice(a1, 2);
        REQUIRE(s1 != s2);
    }

    {
        int a1[] = {2, 3, 4};
        // slices over same data are equal
        auto s1 = nel::Slice(a1, 2);
        auto s2 = nel::Slice(a1, 2);
        REQUIRE(!(s1 != s2));
    }

    {
        int a1[] = {2, 3, 4};
        // slices over diff data are not.
        auto s1 = nel::Slice(a1 + 1, 2);
        auto s2 = nel::Slice(a1, 2);
        REQUIRE(s1 != s2);
    }
    {
        int a1[] = {2, 3, 4};
        // slices over diff data are not.
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1, 2);
        REQUIRE(s1 != s2);
    }

    {
        int a1[] = {2, 3, 4};
        int a2[] = {3, 4, 2};
        // slices over diff data are eq if values are same for same len
        auto s1 = nel::Slice(a1, 2);
        auto s2 = nel::Slice(a2 + 1, 2);
        REQUIRE(s1 != s2);
    }
}

TEST_CASE("Slice::move_from(src)", "[slice]")
{
    {
        // Can move contents of empty slices
        auto s1 = nel::Slice<int>::empty();
        auto s2 = nel::Slice<int>::empty();
        s1.move_from(s2);
    }
    {
        // Can move contents of empty slices
        auto s1 = nel::Slice<int>(nullptr, (nel::Length)0);
        auto s2 = nel::Slice<int>(nullptr, (nel::Length)0);
        s1.move_from(s2);
    }
    {
        int a1[] = {2, 3, 4};
        int a2[] = {3, 4, 2};
        // Can move contents of empty slices
        auto s1 = nel::Slice(a1, (nel::Length)0);
        auto s2 = nel::Slice(a2, (nel::Length)0);
        s1.move_from(s2);
    }

    {
        int a1[] = {2, 3, 4};
        int a2[] = {3, 4, 2};
        // [pod] Can move contents of non-empty slices
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a2, 3);
        s1.move_from(s2);
        REQUIRE(s1[0] == 3);
        REQUIRE(s1[1] == 4);
        REQUIRE(s1[2] == 2);
    }

    {
        int a1[] = {2, 3, 4};
        // [pod] Can move contents of self into self ..
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1, 3);
        s1.move_from(s2);
        REQUIRE(s1[0] == 2);
        REQUIRE(s1[1] == 3);
        REQUIRE(s1[2] == 4);
    }

    // {
    //     int a1[] = {2,3,4,5};
    //     int a2[] = {3,4,2};
    //     // panic if different lengths
    //     // disabled until panic testing available
    //     auto s1 = nel::Slice(a1, 4);
    //     auto s2 = nel::Slice(a2, 3);
    //     s1.move_from(s2);
    // }

    // {
    //     int a1[] = {2,3,4,5};
    //     int a2[] = {3,4,2};
    //     // panic if different lengths
    //     // disabled until panic testing available
    //     auto s1 = nel::Slice(a1, 2);
    //     auto s2 = nel::Slice(a2, 3);
    //     s1.move_from(s2);
    // }

    {
        Stub a1[] = {Stub(2), Stub(3), Stub(4)};
        Stub a2[] = {Stub(3), Stub(4), Stub(2)};
        // [pod] Can move contents of non-empty slices
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a2, 3);

        s1.move_from(s2);

        // src is invalidated
        REQUIRE(!a2[0].valid);
        REQUIRE(!a2[1].valid);
        REQUIRE(!a2[2].valid);

        // dest is valid, and has expected values
        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
        REQUIRE(a1[0].val == 3);
        REQUIRE(a1[1].val == 4);
        REQUIRE(a1[2].val == 2);
    }

    {
        Stub a1[] = {Stub(2), Stub(3), Stub(4)};
        // [udt] Can move contents of self into self ..
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1, 3);
        s1.move_from(s2);
        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
        REQUIRE(a1[0].val == 2);
        REQUIRE(a1[1].val == 3);
        REQUIRE(a1[2].val == 4);
    }

    // {
    //     // Cannot move const contents of empty slices
    //     auto const s1 = nel::Slice<int>::empty();
    //     auto const s2 = nel::Slice<int>::empty();
    //     s1.move_from(s2);
    // }

    // {
    //     Stub const a1[] = {Stub(2),Stub(3),Stub(4)};
    //     // [udt] Cannot move const  contents of self into self ..
    //     auto s1 = nel::Slice(a1, 3);
    //     auto s2 = nel::Slice(a1, 3);
    //     s1.move_from(s2);
    // }

    // {
    //     Stub const a1[] = {Stub(2),Stub(3),Stub(4)};
    //     // [udt] Cannot move const  contents of self into self ..
    //     auto s1 = nel::Slice<Stub const>::from(a1, 3);
    //     auto s2 = nel::Slice<Stub const>::from(a1, 3);
    //     s1.move_from(s2);
    // }

    // {
    //     Stub a1[] = {Stub(2),Stub(3),Stub(4)};
    //     // [udt] Cannot move const  contents of self into self ..
    //     auto const s1 = nel::Slice(a1, 3);
    //     auto const s2 = nel::Slice(a1, 3);
    //     s1.move_from(s2);
    // }
}

TEST_CASE("Slice::copy_from(src)", "[slice]")
{
    {
        // Can copy contents of empty slices
        auto s1 = nel::Slice<int>::empty();
        auto s2 = nel::Slice<int>::empty();
        s1.copy_from(s2);
    }
    {
        // Can copy contents of empty slices
        auto s1 = nel::Slice<int>(nullptr, (nel::Length)0);
        auto s2 = nel::Slice<int>(nullptr, (nel::Length)0);
        s1.copy_from(s2);
    }
    {
        int a1[] = {2, 3, 4};
        int a2[] = {3, 4, 2};
        // Can copy contents of empty slices
        auto s1 = nel::Slice(a1, (nel::Length)0);
        auto s2 = nel::Slice(a2, (nel::Length)0);
        s1.copy_from(s2);
    }

    {
        int a1[] = {2, 3, 4};
        int a2[] = {3, 4, 2};
        // [pod] Can copy contents of non-empty slices
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a2, 3);
        s1.copy_from(s2);
        // dest has now been changed
        REQUIRE(a1[0] == 3);
        REQUIRE(a1[1] == 4);
        REQUIRE(a1[2] == 2);
        // src is left untouched.
        REQUIRE(a2[0] == 3);
        REQUIRE(a2[1] == 4);
        REQUIRE(a2[2] == 2);
    }

    {
        int a1[] = {2, 3, 4};
        // [pod] Can copy contents of self into self ..
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1, 3);
        s1.copy_from(s2);
        REQUIRE(a1[0] == 2);
        REQUIRE(a1[1] == 3);
        REQUIRE(a1[2] == 4);
    }

    // {
    //     int a1[] = {2,3,4,5};
    //     int a2[] = {3,4,2};
    //     // panic if different lengths
    //     // disabled until panic testing available
    //     auto s1 = nel::Slice(a1, 4);
    //     auto s2 = nel::Slice(a2, 3);
    //     s1.copy_from(s2);
    // }

    // {
    //     int a1[] = {2,3,4,5};
    //     int a2[] = {3,4,2};
    //     // panic if different lengths
    //     // disabled until panic testing available
    //     auto s1 = nel::Slice(a1, 2);
    //     auto s2 = nel::Slice(a2, 3);
    //     s1.copy_from(s2);
    // }

    {
        Stub a1[] = {Stub(2), Stub(3), Stub(4)};
        Stub a2[] = {Stub(3), Stub(4), Stub(2)};
        // [pod] Can copy contents of non-empty slices
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a2, 3);

        s1.copy_from(s2);

        // src is still valid
        REQUIRE(a2[0].valid);
        REQUIRE(a2[1].valid);
        REQUIRE(a2[2].valid);
        REQUIRE(a2[0].val == 3);
        REQUIRE(a2[1].val == 4);
        REQUIRE(a2[2].val == 2);

        // dest is valid, and has expected values
        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
        REQUIRE(a1[0].val == 3);
        REQUIRE(a1[1].val == 4);
        REQUIRE(a1[2].val == 2);
    }

    {
        Stub a1[] = {Stub(2), Stub(3), Stub(4)};
        // [udt] Can copy contents of self into self ..
        auto s1 = nel::Slice(a1, 3);
        auto s2 = nel::Slice(a1, 3);
        s1.copy_from(s2);
        REQUIRE(a1[0].valid);
        REQUIRE(a1[1].valid);
        REQUIRE(a1[2].valid);
        REQUIRE(a1[0].val == 2);
        REQUIRE(a1[1].val == 3);
        REQUIRE(a1[2].val == 4);
    }

    // {
    //     // Cannot copy to const contents of empty slices
    //     auto const s1 = nel::Slice<int>::empty();
    //     auto const s2 = nel::Slice<int>::empty();
    //     s1.copy_from(s2);
    // }

    // {
    //     Stub const a1[] = {Stub(2),Stub(3),Stub(4)};
    //     // [udt] Cannot copy to const  contents of self into self ..
    //     auto s1 = nel::Slice(a1, 3);
    //     auto s2 = nel::Slice(a1, 3);
    //     s1.copy_from(s2);
    // }

    // {
    //     Stub const a1[] = {Stub(2),Stub(3),Stub(4)};
    //     // [udt] Cannot copy to  const  contents of self into self ..
    //     auto s1 = nel::Slice<Stub const>::from(a1, 3);
    //     auto s2 = nel::Slice<Stub const>::from(a1, 3);
    //     s1.copy_from(s2);
    // }

    // {
    //     Stub a1[] = {Stub(2),Stub(3),Stub(4)};
    //     // [udt] Cannot copy const  contents of self into self ..
    //     auto const s1 = nel::Slice(a1, 3);
    //     auto const s2 = nel::Slice(a1, 3);
    //     s1.copy_from(s2);
    // }
}

} // namespace slice
} // namespace test
} // namespace nel
