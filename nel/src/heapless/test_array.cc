#include "heapless/array.hh"

#include <catch2/catch.hpp>

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

TEST_CASE("heapless::Array::move", "[heapless][array]")
{
    {
        // empty array can be moved
        auto a2 = nel::heapless::Array<int, 3>::fill(4);
        auto a1 = nel::heapless::Array<int, 3>::fill(1);
        a2 = std::move(a1);
        // heapless arrays can never be empty..
        // how to test array elems are moved then..?
        // all entries in a2 must now be 1
        // TODO: how to drop fold<bool>(..0 to fold(..)
        // std::function<bool(bool, int&)> fn = [](bool acc, int &v)->bool { return acc && v == 1;
        // };
        // std::function fn1 = [](bool acc, int &v) -> bool {
        //     return acc && v == 1;
        // };
        std::function fn1 = [](bool &acc, int &v) {
            acc = acc && (v == 1);
        };
        auto r = a2.iter().fold(true, fn1);
        REQUIRE(r == true);
    }

    {
        // not empty array can be moved
        auto a2 = nel::heapless::Array<int, 3>::fill(4);
        auto a3 = nel::heapless::Array<int, 3>::fill(2);
        a2 = std::move(a3);
        // std::function fn2 = [](bool acc, int &v) -> bool {
        //     return acc && v == 2;
        // };
        std::function fn2 = [](bool &acc, int &v) {
            acc = acc && (v == 2);
        };
        auto r2 = a2.iter().fold(true, fn2);
        REQUIRE(r2 == true);
        // REQUIRE(!a2.is_empty());
        // REQUIRE(a3.is_empty());
        // testing const array moving, but should fail at compile time.
        // auto const c1 = nel::heapless::Array<int, 3>::empty();
        // auto const c2 = nel::heapless::Array<int, 3>::fill(2,1);
        // c2 = std::move(c1);
    }
}

#if 0
TEST_CASE("heapless::Array::is_empty", "[heapless][array]")
{
    // empty array must be empty
    auto a1 = nel::heapless::Array<int, 3>::fill(0);
    //REQUIRE(a1.is_empty());

    auto const c1 = nel::heapless::Array<int, 3>::empty();
    REQUIRE(c1.is_empty());


    // array filled to length 0 must be empty
    auto a2 = nel::heapless::Array<int, 3>::fill(2, 0);
    REQUIRE(a2.is_empty());

    auto const c2 = nel::heapless::Array<int, 3>::fill(2, 0);
    REQUIRE(c2.is_empty());
    // and has no allocations

    // array filled to length >0 must not be empty
    auto a3 = nel::heapless::Array<int, 3>::fill(2, 1);
    REQUIRE(!a3.is_empty());

    auto c3 = nel::heapless::Array<int, 3>::fill(2, 1);
    REQUIRE(!c3.is_empty());
}
#endif

TEST_CASE("heapless::Array::len", "[heapless][array]")
{
    // heapless array cannot be empty
    // TODO: what of N == 0?

    {
        auto a1 = nel::heapless::Array<int, 3>::fill(1);
        REQUIRE(a1.len() == 3);

        auto const c1 = nel::heapless::Array<int, 3>::fill(2);
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
    auto a1 = nel::heapless::Array<int, 3>::fill(0);
    auto sa1 = a1.slice();
    REQUIRE(!sa1.is_empty());
    REQUIRE(sa1.len() == a1.len());

    auto const c1 = nel::heapless::Array<int, 3>::fill(1);
    auto sc1 = c1.slice();
    REQUIRE(!sc1.is_empty());
    REQUIRE(sc1.len() == c1.len());
}

TEST_CASE("heapless::Array::iter()", "[heapless][array]")
{
    // can create iter on non empty arrays.
    auto a2 = nel::heapless::Array<int, 3>::fill(2);
    auto it2 = a2.iter();
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().unwrap() == 2);
    REQUIRE(it2.next().is_none());

    auto const c2 = nel::heapless::Array<int, 3>::fill(2);
    auto itc2 = c2.iter();
    REQUIRE(itc2.next().unwrap() == 2);
    REQUIRE(itc2.next().unwrap() == 2);
    REQUIRE(itc2.next().unwrap() == 2);
    REQUIRE(itc2.next().is_none());
}
