// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/heapless/array.hh>

#include <nel/memory.hh> // movestd::()

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace heapless
{
namespace array
{

#if 0
// cannot have empty in heapless, all array entries must be initialised
// and presumably the same should be true for heaped.
TEST_CASE("heapless::Array::empty", "[heapless][array]")
{
    // must be able to create an empty array..
    auto a1 = nel::heapless::Array<int, 3>::empty();
    auto const c1 = nel::heapless::Array<int, 3>::empty();

    // // empty array must be empty
    // REQUIRE(a1.is_empty());
    // // empty array must have len of 0
    // REQUIRE(a1.len() == 0);

}
#endif

// how to test that array cannot be copied?
// it will fail at compile time.
#if 0
TEST_CASE("heapless::Array::filled", "[heapless][array]")
{
    {
        // must fill array with same value
        auto a1 = nel::heapless::Array<int, 3>::try_from({3, 3, 3}).unwrap();
        REQUIRE(a1.try_get(0).unwrap() == 4);
        REQUIRE(a1.try_get(1).unwrap() == 4);
        REQUIRE(a1.try_get(2).unwrap() == 4);
    }

    {
        auto const c1 = nel::heapless::Array<int, 3>::try_from({5, 5, 5}).unwrap();
        REQUIRE(c1.len() == 3);
        REQUIRE(c1.try_get(0).unwrap() == 5);
        REQUIRE(c1.try_get(1).unwrap() == 5);
        REQUIRE(c1.try_get(2).unwrap() == 5);
    }
}
#endif

#if 0
TEST_CASE("heapless::Array::try_from(initlist)", "[heapless][array]")
{
    {
        // must fill array with same value
        auto a1 = nel::heapless::Array<int, 3>::try_from({1, 2, 3});
        REQUIRE(a1.is_some());
        auto v1 = a1.unwrap();
        REQUIRE(v1.try_get(0).unwrap() == 1);
        REQUIRE(v1.try_get(1).unwrap() == 2);
        REQUIRE(v1.try_get(2).unwrap() == 3);
    }

    {
        // init list too small: must fail
        auto a1 = nel::heapless::Array<int, 3>::try_from({1, 2});
        REQUIRE(a1.is_none());
    }

    {
        // init list too large: must fail
        auto a1 = nel::heapless::Array<int, 3>::try_from({1, 2, 3, 4});
        REQUIRE(a1.is_none());
    }
}
#endif

TEST_CASE("heapless::heapless::Array::move", "[heapless][array]")
{
    {
        // empty array can be moved
        // auto a2 = nel::heapless::Array<int, 3>::try_from({3, 3, 3}).unwrap();
        // auto a1 = nel::heapless::Array<int, 3>::try_from({1, 1, 1}).unwrap();
        auto a2 = nel::heapless::Array<int, 3>::filled_with(3);
        auto a1 = nel::heapless::Array<int, 3>::filled_with(1);
        a2 = nel::move(a1);
        // heapless arrays can never be empty..
        // how to test array elems are moved then..?
        // all entries in a2 must now be 1
        // TODO: how to drop fold<bool>(..0 to fold(..)
        auto r = a2.iter().fold(true, [](bool &acc, int const &v) { acc = acc && (v == 1); });
        REQUIRE(r == true);
    }

    {
        // not empty array can be moved
        // auto a2 = nel::heapless::Array<int, 3>::try_from({4, 4, 4}).unwrap();
        // auto a3 = nel::heapless::Array<int, 3>::try_from({2, 2, 2}).unwrap();
        auto a2 = nel::heapless::Array<int, 3>::filled_with(4);
        auto a3 = nel::heapless::Array<int, 3>::filled_with(2);
        a2 = nel::move(a3);
        auto r2 = a2.iter().fold(true, [](bool &acc, int const &v) { acc = acc && (v == 2); });
        REQUIRE(r2 == true);
        // REQUIRE(!a2.is_empty());
        // REQUIRE(a3.is_empty());
        // testing const array moving, but should fail at compile time.
        // auto const c1 = nel::heapless::Array<int, 3>::empty();
        // auto const c2 = nel::heapless::Array<int, 3>::try_from(2,1).unwrap();
        // c2 = nel::move(c1);
    }
}

TEST_CASE("heapless::Array::is_empty", "[heapless][array]")
{
    // Arrays cannot be empty..
    // cannot have 0 sized array in C++, compiler will refuse to compile.

    // auto a1 = nel::heapless::Array<int, 3>::try_from({0, 0, 0}).unwrap();
    auto a1 = nel::heapless::Array<int, 3>::filled_with(0);
    REQUIRE(!a1.is_empty());

    // auto const c1 = nel::heapless::Array<int, 3>::try_from({2, 2, 2}).unwrap();
    auto const c1 = nel::heapless::Array<int, 3>::filled_with(2);
    REQUIRE(!c1.is_empty());
}

TEST_CASE("heapless::Array::len", "[heapless][array]")
{
    // heapless array cannot be empty
    // TODO: what of N == 0?

    {
        // auto a1 = nel::heapless::Array<int, 3>::try_from({1, 1, 1}).unwrap();
        auto a1 = nel::heapless::Array<int, 3>::filled_with(1);
        REQUIRE(a1.len() == 3);

        auto const c1 = nel::heapless::Array<int, 3>::filled_with(1);
        REQUIRE(c1.len() == 3);
    }
}

// should this be .into_slice()?
// is this a conversion, or an as.
TEST_CASE("heapless::Array::slice()", "[heapless][array]")
{
    // what of array of length 0?
    // auto a1 = nel::heapless::Array<int, 0>::??;

    // full slice of array is all of it.
    // auto a1 = nel::heapless::Array<int, 3>::try_from({0, 0, 0}).unwrap();
    auto a1 = nel::heapless::Array<int, 3>::filled_with(0);
    auto sa1 = a1.slice();
    REQUIRE(!sa1.is_empty());
    REQUIRE(sa1.len() == a1.len());

    // auto const c1 = nel::heapless::Array<int, 3>::try_from({1, 1, 1}).unwrap();
    auto const c1 = nel::heapless::Array<int, 3>::filled_with(1);
    auto sc1 = c1.slice();
    REQUIRE(!sc1.is_empty());
    REQUIRE(sc1.len() == c1.len());
}

TEST_CASE("heapless::Array::slice(b,e)", "[heapless][array]")
{
    {
        // sub slice of array is empty.
        // auto a1 = nel::heapless::Array<int, 3>::try_from({3, 3, 3}).unwrap();
        auto a1 = nel::heapless::Array<int, 3>::filled_with(3);
        auto sa1 = a1.slice(0, 0);
        REQUIRE(sa1.is_empty());
        REQUIRE(sa1.len() == 0);

        // in-range slice is not empty
        auto sa12 = a1.slice(0, 1);
        REQUIRE(!sa12.is_empty());
        REQUIRE(sa12.len() == 1);

        // out-of-range slice is empty
        auto sa13 = a1.slice(3, 4);
        REQUIRE(sa13.is_empty());

        // partially out-of-range slice is not empty, and has only up to valid items
        auto sa14 = a1.slice(2, 4);
        REQUIRE(!sa14.is_empty());

        // partially out-of-range slice is not empty, and has only up to valid items
        auto sa15 = a1.slice(0, 4);
        REQUIRE(!sa15.is_empty());
        REQUIRE(sa15.len() == 3);
    }

    {
        // auto const c1 = nel::heapless::Array<int, 3>::try_from({5, 5, 5}).unwrap();
        auto const c1 = nel::heapless::Array<int, 3>::filled_with(5);
        auto sc1 = c1.slice(0, 0);
        REQUIRE(sc1.is_empty());

        // sub slice of non-empty array is not empty.
        auto sc2 = c1.slice(0, 1);
        REQUIRE(!sc2.is_empty());
        REQUIRE(sc2.len() == 1);

        auto sc3 = c1.slice(3, 4);
        REQUIRE(sc3.is_empty());

        auto sc4 = c1.slice(2, 4);
        REQUIRE(!sc4.is_empty());
        REQUIRE(sc4.len() == 1);

        auto sc5 = c1.slice(0, 4);
        REQUIRE(!sc5.is_empty());
        REQUIRE(sc5.len() == 3);
    }
}

TEST_CASE("heapless::Array::iter()", "[heapless][array]")
{
    // can create iter on non empty arrays.
    // auto a2 = nel::heapless::Array<int, 3>::try_from({2, 2, 2}).unwrap();
    auto a2 = nel::heapless::Array<int, 3>::filled_with(2);
    auto it2 = a2.iter();
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().is_none());

    // auto const c2 = nel::heapless::Array<int, 3>::try_from({3, 3, 3}).unwrap();
    auto const c2 = nel::heapless::Array<int, 3>::filled_with(3);
    auto itc2 = c2.iter();
    REQUIRE(itc2.next().unwrap() == 3);
    REQUIRE(itc2.next().unwrap() == 3);
    REQUIRE(itc2.next().unwrap() == 3);
    REQUIRE(itc2.next().is_none());
}

TEST_CASE("heapless::Array::try_get", "[heapless][array]")
{
    {
        // auto a1 = nel::heapless::Array<int, 3>::try_from({3, 3, 3}).unwrap();
        auto a1 = nel::heapless::Array<int, 3>::filled_with(3);

        // in-range get is a value
        auto ra1 = a1.try_get(0);
        REQUIRE(ra1.is_some());
        REQUIRE(ra1.unwrap() == 3);

        // out-of-range get is none
        auto ra2 = a1.try_get(3);
        REQUIRE(ra2.is_none());
    }

    {
        // auto const c1 = nel::heapless::Array<int, 3>::try_from({5, 5, 5}).unwrap();
        auto const c1 = nel::heapless::Array<int, 3>::filled_with(5);
        // in-range get is a value
        auto rc1 = c1.try_get(0);
        REQUIRE(rc1.is_some());
        REQUIRE(rc1.unwrap() == 5);

        // out-of-range get is none
        auto rc2 = c1.try_get(3);
        REQUIRE(rc2.is_none());
    }
}

}; // namespace array
}; // namespace heapless
}; // namespace test
}; // namespace nel
