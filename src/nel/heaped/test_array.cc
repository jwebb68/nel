// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/heaped/array.hh>

#include <nel/memory.hh> // nel::move()

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace heaped
{
namespace array
{

TEST_CASE("heaped::Array::empty", "[heaped][array]")
{
    // must be able to create an empty array..
    auto a1 = nel::heaped::Array<int>::empty();
    auto const c1 = nel::heaped::Array<int>::empty();

    // // empty array must be empty
    // REQUIRE(a1.is_empty());
    // // empty array must have len of 0
    // REQUIRE(a1.len() == 0);
}

// how to test that array cannot be copied?
// it will fail at compile time.

TEST_CASE("heaped::Array::move", "[heaped][array]")
{
    // empty array can be moved
    nel::heaped::Array<int> a1 = nel::heaped::Array<int>::empty();
    // nel::heaped::Array<int> a2 = nel::heaped::Array<int>::try_from({2}).unwrap();
    auto a2 = nel::heaped::Array<int>::filled(2, 1);
    a2 = nel::move(a1);
    REQUIRE(a1.is_empty());
    REQUIRE(a2.is_empty());

    // not empty array can be moved
    // auto a3 = nel::heaped::Array<int>::try_from({2}).unwrap();
    auto a3 = nel::heaped::Array<int>::filled(2, 4);
    a2 = nel::move(a3);
    REQUIRE(!a2.is_empty());
    REQUIRE(a3.is_empty());

    // testing const array moving, but should fail at compile time.
    // auto const c1 = nel::heaped::Array<int>::empty();
    // auto const c2 = nel::heaped::Array<int>::filled(2,1);
    // c2 = nel::move(c1);
}

#if 0
TEST_CASE("heaped::Array::try_from(initlist)", "[heaped][array]")
{
    {
        // must fill array with same value
        auto a1 = nel::heaped::Array<int>::try_from({1, 2, 3});
        REQUIRE(a1.is_some());
        auto v1 = a1.unwrap();
        REQUIRE(v1.try_get(0).unwrap() == 1);
        REQUIRE(v1.try_get(1).unwrap() == 2);
        REQUIRE(v1.try_get(2).unwrap() == 3);
    }
}
#endif

TEST_CASE("heaped::Array::filled()", "[heaped][array]")
{
    // filled creates 0len array if filled with 0 elements.
    {
        auto a1 = nel::heaped::Array<int>::filled(2, 0);
        REQUIRE(a1.is_empty());
        REQUIRE(a1.len() == 0);
    }

    // filled creates 1len array if filled with 1 elements.
    {
        auto a1 = nel::heaped::Array<int>::filled(1, 1);
        REQUIRE(!a1.is_empty());
        REQUIRE(a1.len() == 1);
        REQUIRE(a1.try_get(0).unwrap() == 1);
    }

    {
        // must fill array with same value
        auto a1 = nel::heaped::Array<int>::filled(2, 3);
        REQUIRE(a1.try_get(0).unwrap() == 2);
        REQUIRE(a1.try_get(1).unwrap() == 2);
        REQUIRE(a1.try_get(2).unwrap() == 2);
    }
}

TEST_CASE("heaped::Array::is_empty", "[heaped][array]")
{
    // empty array must be empty
    auto a1 = nel::heaped::Array<int>::empty();
    REQUIRE(a1.is_empty());

    auto const c1 = nel::heaped::Array<int>::empty();
    REQUIRE(c1.is_empty());

#if 0
    // array filled to length 0 must be empty
    auto a2 = nel::heaped::Array<int>::try_from({}).unwrap();
    REQUIRE(a2.is_empty());

    auto const c2 = nel::heaped::Array<int>::try_from({}).unwrap();
    REQUIRE(c2.is_empty());
    // and has no allocations
#endif

    // array filled to length 0 must be empty
    auto a3 = nel::heaped::Array<int>::filled(2, 0);
    REQUIRE(a3.is_empty());

    auto const c3 = nel::heaped::Array<int>::filled(2, 0);
    REQUIRE(c3.is_empty());
    // and has no allocations

    // array filled to length >0 must not be empty
    // auto a3 = nel::heaped::Array<int>::try_from({2}).unwrap();
    auto a4 = nel::heaped::Array<int>::filled(2, 1);
    REQUIRE(!a4.is_empty());

    // auto const c4 = nel::heaped::Array<int>::try_from({2}).unwrap();
    auto const c4 = nel::heaped::Array<int>::filled(2, 1);
    REQUIRE(!c4.is_empty());
}

TEST_CASE("heaped::Array::len", "[heaped][array]")
{
    {
        // empty array must have len 0
        auto a1 = nel::heaped::Array<int>::empty();
        REQUIRE(a1.len() == 0);

        auto const c1 = nel::heaped::Array<int>::empty();
        REQUIRE(c1.len() == 0);
    }

#if 0
    // array filled to length 0 must have len 0
    {
        auto a2 = nel::heaped::Array<int>::try_from({}).unwrap();
        REQUIRE(a2.len() == 0);

        auto const c2 = nel::heaped::Array<int>::try_from({}).unwrap();
        REQUIRE(c2.len() == 0);
    }
#endif

    {
        // array filled to length 0 must have len 0
        auto a3 = nel::heaped::Array<int>::filled(2, 0);
        REQUIRE(a3.len() == 0);

        // auto const c3 = nel::heaped::Array<int>::try_from({}).unwrap();
        auto const c3 = nel::heaped::Array<int>::filled(3, 0);
        REQUIRE(c3.len() == 0);
    }

    {
        // array filled to length >0 must have len of length
        // auto a4 = nel::heaped::Array<int>::try_from({2}).unwrap();
        auto a4 = nel::heaped::Array<int>::filled(3, 1);
        REQUIRE(a4.len() == 1);

        // auto const c4 = nel::heaped::Array<int>::try_from({2}).unwrap();
        auto const c4 = nel::heaped::Array<int>::filled(2, 1);
        REQUIRE(c4.len() == 1);
    }
}

// TODO Array::try_get(index).

// should this be .into_slice()?
// is this a conversion, or an as.
TEST_CASE("heaped::Array::slice()", "[heaped][array]")
{
    // full slice of empty array is empty.
    auto a1 = nel::heaped::Array<int>::empty();
    auto sa1 = a1.slice();
    REQUIRE(sa1.is_empty());

    auto const c1 = nel::heaped::Array<int>::empty();
    auto sc1 = c1.slice();
    REQUIRE(sc1.is_empty());

    // full slice of non-empty array is not empty.
    // auto a2 = nel::heaped::Array<int>::try_from({2}).unwrap();
    auto a2 = nel::heaped::Array<int>::filled(2, 1);
    auto sa2 = a2.slice();
    REQUIRE(!sa2.is_empty());
    REQUIRE(sa2.len() == 1);

    // auto const c2 = nel::heaped::Array<int>::try_from({2}).unwrap();
    auto const c2 = nel::heaped::Array<int>::filled(2, 1);
    auto sc2 = c2.slice();
    REQUIRE(!sc2.is_empty());
    REQUIRE(sc2.len() == 1);

    // full slice of non-empty array is not empty.
    // auto a3 = nel::heaped::Array<int>::try_from({2}).unwrap();
    auto a3 = nel::heaped::Array<int>::filled(2, 1);
    auto sa3 = a3.slice();
    REQUIRE(!sa3.is_empty());
    REQUIRE(sa3.len() == 1);
}

TEST_CASE("heaped::Array::slice(b,e)", "[heaped][array]")
{
    {
        // sub slice of empty array is empty.
        auto a1 = nel::heaped::Array<int>::empty();
        auto sa1 = a1.slice(0, 1);
        REQUIRE(sa1.is_empty());

        auto const c1 = nel::heaped::Array<int>::empty();
        auto sc1 = c1.slice(0, 1);
        REQUIRE(sc1.is_empty());
    }

    {
        // sub slice of array is empty.
        // auto a1 = nel::heaped::Array<int>::try_from({3, 3, 3}).unwrap();
        auto a1 = nel::heaped::Array<int>::filled(3, 3);

        // auto a1 = nel::heaped::Array<int>::filled(3, 3);
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
        auto const c1 = nel::heaped::Array<int>::filled(3, 3);
        // auto const c1 = nel::heaped::Array<int>::filled(5, 3);
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

TEST_CASE("heaped::Array::iter()", "[heaped][array]")
{
#if defined(RUST_LIKE)
    // can create iter on empty arrays.
    auto a1 = nel::heaped::Array<int>::empty();
    auto it1 = a1.iter();
    // and the iter is empty.
    REQUIRE(it1.next().is_none());

    auto const c1 = nel::heaped::Array<int>::empty();
    auto itc1 = c1.iter();
    // and the iter is empty.
    REQUIRE(itc1.next().is_none());

    // can create iter on non empty arrays.
    // auto a2 = nel::heaped::Array<int>::try_from({2, 2}).unwrap();
    auto a2 = nel::heaped::Array<int>::filled(2, 2);

    // auto a2 = nel::heaped::Array<int>::filled(2, 2);
    auto it2 = a2.iter();
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().is_none());

    auto const c2 = nel::heaped::Array<int>::filled(2, 2);
    auto itc2 = c2.iter();
    REQUIRE(itc2.next().unwrap() == 2);
    REQUIRE(itc2.next().unwrap() == 2);
    REQUIRE(itc2.next().is_none());
#endif
}

TEST_CASE("heaped::Array::try_get", "[heaped][array]")
{
    {
        // get on an empty array gets nothing.
        auto a1 = nel::heaped::Array<int>::empty();
        auto sa1 = a1.try_get(0);
        REQUIRE(sa1.is_none());

        auto const c1 = nel::heaped::Array<int>::empty();
        auto sc1 = c1.try_get(0);
        REQUIRE(sc1.is_none());
    }

    {
        // in-range get on non-empty array is not empty.
        // auto a2 = nel::heaped::Array<int>::try_from({2, 2, 2}).unwrap();
        auto a2 = nel::heaped::Array<int>::filled(2, 3);

        // auto a2 = nel::heaped::Array<int>::filled(2, 3);
        auto sa2 = a2.try_get(0);
        REQUIRE(sa2.is_some());
        REQUIRE(sa2.unwrap() == 2);

        // auto const c2 = nel::heaped::Array<int>::try_from({2, 2, 2}).unwrap();
        auto const c2 = nel::heaped::Array<int>::filled(2, 3);
        auto sc2 = c2.try_get(0);
        REQUIRE(sc2.is_some());
        REQUIRE(sc2.unwrap() == 2);
    }

    {
        // out-of-range get on non-empty array must return none.
        // auto a2 = nel::heaped::Array<int>::try_from({2, 2, 2}).unwrap();
        auto a2 = nel::heaped::Array<int>::filled(2, 3);

        // auto a2 = nel::heaped::Array<int>::filled(2, 3);
        auto sa2 = a2.try_get(3);
        REQUIRE(sa2.is_none());

        // auto const c2 = nel::heaped::Array<int>::try_from({2, 2, 2}).unwrap();
        auto const c2 = nel::heaped::Array<int>::filled(2, 3);
        auto sc2 = c2.try_get(3);
        REQUIRE(sc2.is_none());
    }
}

}; // namespace array
}; // namespace heaped
}; // namespace test
}; // namespace nel
