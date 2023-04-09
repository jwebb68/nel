// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "slice.hh"

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace slice
{

TEST_CASE("Slice::empty()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    REQUIRE(s1.is_empty());

    auto const c1 = nel::Slice<int>::empty();
    REQUIRE(c1.is_empty());
}

TEST_CASE("Slice::from(ptr,len)", "[slice]")
{
    int a1[] = {3, 1, 2, 4};
    auto s1 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    REQUIRE(s1.len() == 4);

    auto const c1 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    REQUIRE(c1.len() == 4);
}

TEST_CASE("Slice::is_empty()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    REQUIRE(s1.is_empty());

    auto const c1 = nel::Slice<int>::empty();
    REQUIRE(c1.is_empty());

    int a1[] = {3, 1, 2, 4};
    auto s2 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    REQUIRE(!s2.is_empty());

    auto const c2 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    REQUIRE(!c2.is_empty());
}

TEST_CASE("Slice::len()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    REQUIRE(s1.len() == 0);

    auto const c1 = nel::Slice<int>::empty();
    REQUIRE(c1.len() == 0);

    int a1[] = {3, 1, 2, 4};
    auto s2 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    REQUIRE(s2.len() == 4);

    auto const c2 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    REQUIRE(c2.len() == 4);
}

TEST_CASE("Slice::fill()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    s1.fill(2);

    // must fail to compile.
    // auto const c1 = nel::Slice<int>::empty();
    // c1.fill(2);

    int a1[] = {3, 1, 2, 4};
    auto s2 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    s2.fill(2);
    REQUIRE(a1[0] == 2);
    REQUIRE(a1[1] == 2);
    REQUIRE(a1[2] == 2);
    REQUIRE(a1[3] == 2);
}

TEST_CASE("Slice::slice(b,e)", "[heapless][vector]")
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
        auto s1 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));

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
        auto s1 = nel::Slice<int const>::from(a1, sizeof(a1) / sizeof(a1[0]));

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
    auto s1 = nel::Slice<int>::empty();
    auto is1 = s1.iter();
    REQUIRE(is1.next().is_none());

    auto const c1 = nel::Slice<int>::empty();
    auto ic1 = c1.iter();
    REQUIRE(ic1.next().is_none());

    int a1[] = {3, 1, 2, 4};
    auto s2 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    auto is2 = s2.iter();

    REQUIRE(is2.next().unwrap() == 3);
    REQUIRE(is2.next().unwrap() == 1);
    REQUIRE(is2.next().unwrap() == 2);
    REQUIRE(is2.next().unwrap() == 4);
    REQUIRE(is2.next().is_none());

    auto const c2 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
    auto ic2 = c2.iter();

    REQUIRE(ic2.next().unwrap() == 3);
    REQUIRE(ic2.next().unwrap() == 1);
    REQUIRE(ic2.next().unwrap() == 2);
    REQUIRE(ic2.next().unwrap() == 4);
    REQUIRE(ic2.next().is_none());

    auto is3 = s2.iter();
    is3.next().unwrap() = 5;
    auto is4 = s2.iter();
    REQUIRE(is4.next().unwrap() == 5);
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
        auto s1 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
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
        auto s1 = nel::Slice<int>::from(a1, sizeof(a1) / sizeof(a1[0]));
        auto sa2 = s1.try_get(5);
        REQUIRE(sa2.is_none());

        auto const cs1 = move(s1);
        auto scs1 = cs1.try_get(5);
        REQUIRE(scs1.is_none());
    }
}

}; // namespace slice
}; // namespace test
}; // namespace nel
