#include <nel/heaped/array.hh>

#include <catch2/catch.hpp>

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
    auto a1 = nel::heaped::Array<int>::empty();
    auto a2 = nel::heaped::Array<int>::fill(2, 1);
    a2 = std::move(a1);
    REQUIRE(a1.is_empty());
    REQUIRE(a2.is_empty());

    // not empty array can be moved
    auto a3 = nel::heaped::Array<int>::fill(2, 1);
    a2 = std::move(a3);
    REQUIRE(!a2.is_empty());
    REQUIRE(a3.is_empty());

    // testing const array moving, but should fail at compile time.
    // auto const c1 = nel::heaped::Array<int>::empty();
    // auto const c2 = nel::heaped::Array<int>::fill(2,1);
    // c2 = std::move(c1);
}

TEST_CASE("heaped::Array::is_empty", "[heaped][array]")
{
    // empty array must be empty
    auto a1 = nel::heaped::Array<int>::empty();
    REQUIRE(a1.is_empty());

    auto const c1 = nel::heaped::Array<int>::empty();
    REQUIRE(c1.is_empty());

    // array filled to length 0 must be empty
    auto a2 = nel::heaped::Array<int>::fill(2, 0);
    REQUIRE(a2.is_empty());

    auto const c2 = nel::heaped::Array<int>::fill(2, 0);
    REQUIRE(c2.is_empty());
    // and has no allocations

    // array filled to length >0 must not be empty
    auto a3 = nel::heaped::Array<int>::fill(2, 1);
    REQUIRE(!a3.is_empty());

    auto c3 = nel::heaped::Array<int>::fill(2, 1);
    REQUIRE(!c3.is_empty());
}

TEST_CASE("heaped::Array::len", "[heaped][array]")
{
    // empty array must have len 0
    auto a1 = nel::heaped::Array<int>::empty();
    REQUIRE(a1.len() == 0);

    auto const c1 = nel::heaped::Array<int>::empty();
    REQUIRE(c1.len() == 0);

    // array filled to length 0 must have len 0
    auto a2 = nel::heaped::Array<int>::fill(2, 0);
    REQUIRE(a2.len() == 0);

    auto const c2 = nel::heaped::Array<int>::fill(2, 0);
    REQUIRE(c2.len() == 0);

    // array filled to length >0 must have len of length
    auto a3 = nel::heaped::Array<int>::fill(2, 1);
    REQUIRE(a3.len() == 1);

    auto const c3 = nel::heaped::Array<int>::fill(2, 1);
    REQUIRE(c3.len() == 1);
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
    auto a2 = nel::heaped::Array<int>::fill(2, 1);
    auto sa2 = a2.slice();
    REQUIRE(!sa2.is_empty());
    REQUIRE(sa2.len() == 1);

    auto const c2 = nel::heaped::Array<int>::fill(2, 1);
    auto sc2 = c2.slice();
    REQUIRE(!sc2.is_empty());
    REQUIRE(sc2.len() == 1);
}

TEST_CASE("heaped::Array::iter()", "[heaped][array]")
{
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
    auto a2 = nel::heaped::Array<int>::fill(2, 2);
    auto it2 = a2.iter();
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().is_none());

    auto const c2 = nel::heaped::Array<int>::fill(2, 2);
    auto itc2 = c2.iter();
    REQUIRE(itc2.next().unwrap() == 2);
    REQUIRE(itc2.next().unwrap() == 2);
    REQUIRE(itc2.next().is_none());
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
        auto a2 = nel::heaped::Array<int>::fill(2, 3);
        auto sa2 = a2.try_get(0);
        REQUIRE(sa2.is_some());
        REQUIRE(sa2.unwrap() == 2);

        auto const c2 = nel::heaped::Array<int>::fill(2, 3);
        auto sc2 = c2.try_get(0);
        REQUIRE(sc2.is_some());
        REQUIRE(sc2.unwrap() == 2);
    }

    {
        // out-of-range get on non-empty array must return none.
        auto a2 = nel::heaped::Array<int>::fill(2, 3);
        auto sa2 = a2.try_get(3);
        REQUIRE(sa2.is_none());

        auto const c2 = nel::heaped::Array<int>::fill(2, 3);
        auto sc2 = c2.try_get(3);
        REQUIRE(sc2.is_none());
    }
}
