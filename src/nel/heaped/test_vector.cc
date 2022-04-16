#include <nel/heaped/vector.hh>

#include <catch2/catch.hpp>

TEST_CASE("heaped::Vector::empty", "[heaped][vector]")
{
    // must be able to create an empty vector..
    auto a1 = nel::heaped::Vector<int>::empty();
    auto const c1 = nel::heaped::Vector<int>::empty();

    // // empty vector must be empty
    // REQUIRE(a1.is_empty());
    // // empty vector must have len of 0
    // REQUIRE(a1.len() == 0);
}

TEST_CASE("heaped::Vector::with_capacity", "[heaped][vector]")
{
    {
        // must be able to create a vector with 0 cap..
        auto a1 = nel::heaped::Vector<int>::with_capacity(0);
        auto const c1 = nel::heaped::Vector<int>::with_capacity(0);
    }

    {
        // must be able to create a vector with !0 cap
        auto a2 = nel::heaped::Vector<int>::with_capacity(10);
        auto const c2 = nel::heaped::Vector<int>::with_capacity(10);
    }
    // // empty vector must be empty
    // REQUIRE(a1.is_empty());
    // // empty vector must have len of 0
    // REQUIRE(a1.len() == 0);
}

// how to test that Vector cannot be copied?
// it will fail at compile time.

TEST_CASE("heaped::Vector::move", "[heaped][vector]")
{
    {
        // empty Vector can be moved
        auto a1 = nel::heaped::Vector<int>::empty();
        auto a2 = std::move(a1);
        REQUIRE(a1.is_empty());
        REQUIRE(a2.is_empty());
    }

    {
        // not empty vector can be moved
        auto a3 = nel::heaped::Vector<int>::empty();
        a3.push_back(2);

        auto a2 = std::move(a3);
        REQUIRE(!a2.is_empty());
        REQUIRE(a3.is_empty());
    }

    {
        // testing const Vector moving, but should fail at compile time.
        // auto const c1 = nel::heaped::Vector<int>::empty();
        // auto const c2 = nel::heaped::Vector<int>::fill(2,1);
        // c2 = std::move(c1);
    }
}

TEST_CASE("heaped::Vector::is_empty", "[heaped][vector]")
{
    {
        // empty vector must be empty
        auto a1 = nel::heaped::Vector<int>::empty();
        REQUIRE(a1.is_empty());

        auto const c1 = nel::heaped::Vector<int>::empty();
        REQUIRE(c1.is_empty());
    }

    {
        // vector filled to length 0 must be empty
        auto a1 = nel::heaped::Vector<int>::fill(2, 0);
        REQUIRE(a1.is_empty());

        auto const c1 = nel::heaped::Vector<int>::fill(2, 0);
        REQUIRE(c1.is_empty());
    }

    {
        // vector filled to length >0 must not be empty
        auto a1 = nel::heaped::Vector<int>::fill(2, 1);
        REQUIRE(!a1.is_empty());

        auto c1 = nel::heaped::Vector<int>::fill(2, 1);
        REQUIRE(!c1.is_empty());
    }

    {
        // vectors with initial capacity are created empty.
        auto a1 = nel::heaped::Vector<int>::with_capacity(10);
        REQUIRE(a1.is_empty());
    }
}

TEST_CASE("heaped::Vector::capacity", "[heaped][Vector]")
{
    {
        // empty vector must have capacity 0
        auto a1 = nel::heaped::Vector<int>::empty();
        REQUIRE(a1.capacity() == 0);

        auto const c1 = nel::heaped::Vector<int>::empty();
        REQUIRE(c1.capacity() == 0);
    }

    {
        // init filled vecs have capacity of fill.
        // vector filled to length 0 must have len 0
        auto a2 = nel::heaped::Vector<int>::fill(2, 0);
        REQUIRE(a2.capacity() == 0);

        auto const c2 = nel::heaped::Vector<int>::fill(2, 0);
        REQUIRE(c2.capacity() == 0);
    }

    {
        // vector filled to length >0 must have len of length
        auto a3 = nel::heaped::Vector<int>::fill(2, 1);
        REQUIRE(a3.capacity() == 1);

        auto const c3 = nel::heaped::Vector<int>::fill(2, 1);
        REQUIRE(c3.capacity() == 1);
    }

    {
        // with initial capacity
        // may not use exact value..
        // but must not be less than.
        auto a4 = nel::heaped::Vector<int>::with_capacity(10);
        REQUIRE(a4.capacity() >= 10);
    }
}

TEST_CASE("heaped::Vector::len", "[heaped][Vector]")
{
    {
        // empty vector must have len 0
        auto a1 = nel::heaped::Vector<int>::empty();
        REQUIRE(a1.len() == 0);

        auto const c1 = nel::heaped::Vector<int>::empty();
        REQUIRE(c1.len() == 0);
    }

    {
        // vector filled to length 0 must have len 0
        auto a2 = nel::heaped::Vector<int>::fill(2, 0);
        REQUIRE(a2.len() == 0);

        auto const c2 = nel::heaped::Vector<int>::fill(2, 0);
        REQUIRE(c2.len() == 0);
    }

    {
        // vector filled to length >0 must have len of length
        auto a3 = nel::heaped::Vector<int>::fill(2, 1);
        REQUIRE(a3.len() == 1);

        auto const c3 = nel::heaped::Vector<int>::fill(2, 1);
        REQUIRE(c3.len() == 1);
    }
}

TEST_CASE("heaped::Vector::clear", "[heaped][Vector]")
{
    {
        // empty vector can be cleared
        nel::Count cap;
        auto a1 = nel::heaped::Vector<int>::empty();
        cap = a1.capacity();
        a1.clear();
        REQUIRE(a1.is_empty());
        REQUIRE(a1.capacity() == cap);

        // const vecs cannot be cleared..
        // auto const c1 = nel::heaped::Vector<int>::empty();
        // c1.clear();
        // REQUIRE(c1.is_empty());
    }

    {
        // vector filled to length 0 must have len 0
        nel::Count cap;
        auto a2 = nel::heaped::Vector<int>::fill(2, 0);
        cap = a2.capacity();
        a2.clear();
        REQUIRE(a2.is_empty());
        REQUIRE(a2.capacity() == cap);

        // auto const c2 = nel::heaped::Vector<int>::fill(2, 0);
        // cap = c2.capacity();
        // c2.clear();
        // REQUIRE(c2.is_empty());
        // REQUIRE(c2.capacity() == cap);
    }

    {
        // vector filled to length >0 must have len of length
        nel::Count cap;
        auto a3 = nel::heaped::Vector<int>::fill(2, 1);
        cap = a3.capacity();
        a3.clear();
        REQUIRE(a3.is_empty());
        REQUIRE(a3.capacity() == cap);

        // auto const c3 = nel::heaped::Vector<int>::fill(2, 1);
        // cap = c3.capacity();
        // c3.clear();
        // REQUIRE(c3.is_empty());
        // REQUIRE(c3.capacity() == cap);
    }
}

TEST_CASE("heaped::Vector::reserve", "[heaped][Vector]")
{
    // reserve, gives the feel of an increment, i.e. the extra capacity
    // above the current len.
    // reserve(10) would set cap to len + 10,
    // reserve(13) would set cap to len + 13,
    // and reserve(0) would shrinkwrap.

    {
        auto a1 = nel::heaped::Vector<int>::empty();

        a1.reserve(10);
        REQUIRE(a1.capacity() >= a1.len() + 10);

        // growing..
        a1.reserve(16);
        REQUIRE(a1.capacity() >= a1.len() + 16);

        // shrinking is not optional..
        // but may not shrink as much as wanted..
        a1.reserve(8);
        REQUIRE(a1.capacity() >= a1.len() + 8);

        // shrinking is not optional..
        // reserving to 0 is shrink-wrapping?
        a1.reserve(0);
        REQUIRE(a1.capacity() >= a1.len() + 0);
    }

    {
        auto a1 = nel::heaped::Vector<int>::fill(2, 10);

        a1.reserve(10);
        REQUIRE(a1.capacity() >= a1.len() + 10);

        // growing..
        a1.reserve(16);
        REQUIRE(a1.capacity() >= a1.len() + 16);

        // shrinking is not optional..
        // but may not shrink as much as wanted..
        a1.reserve(8);
        REQUIRE(a1.capacity() >= a1.len() + 8);

        // shrinking is not optional..
        // reserving to 0 is shrink-wrapping?
        a1.reserve(0);
        REQUIRE(a1.capacity() >= a1.len() + 0);
    }
}

// TODO Array::try_get(index).

// should this be .into_slice()?
// is this a conversion, or an as.
TEST_CASE("heaped::Vector::slice()", "[heaped][Vector]")
{
    {
        // full slice of empty vector is empty.
        auto a1 = nel::heaped::Vector<int>::empty();
        auto sa1 = a1.slice();
        REQUIRE(sa1.is_empty());

        auto const c1 = nel::heaped::Vector<int>::empty();
        auto sc1 = c1.slice();
        REQUIRE(sc1.is_empty());
    }

    {
        // full slice of non-empty vector is not empty.
        auto a2 = nel::heaped::Vector<int>::fill(2, 1);
        auto sa2 = a2.slice();
        REQUIRE(!sa2.is_empty());
        REQUIRE(sa2.len() == 1);

        auto const c2 = nel::heaped::Vector<int>::fill(2, 1);
        auto sc2 = c2.slice();
        REQUIRE(!sc2.is_empty());
        REQUIRE(sc2.len() == 1);
    }
}

TEST_CASE("heaped::Vector::subslice(b,e)", "[heaped][vector]")
{
    {
        // sub slice of empty vec is empty.
        auto a1 = nel::heaped::Vector<int>::empty();
        auto sa1 = a1.subslice(0, 0);
        REQUIRE(sa1.is_empty());
        REQUIRE(sa1.len() == 0);

        auto sa2 = a1.subslice(3, 5);
        REQUIRE(sa2.is_empty());
        REQUIRE(sa2.len() == 0);
    }

    {
        // in-range subslice is not empty
        auto a1 = nel::heaped::Vector<int>::empty();
        a1.push_back(1);
        a1.push_back(2);

        auto sa12 = a1.subslice(0, 1);
        REQUIRE(!sa12.is_empty());
        REQUIRE(sa12.len() == 1);

        // out-of-range subslice is empty
        auto sa13 = a1.subslice(3, 4);
        REQUIRE(sa13.is_empty());

        // partially out-of-range subslice is not empty, and has only up to valid items
        auto sa14 = a1.subslice(1, 4);
        REQUIRE(!sa14.is_empty());
        REQUIRE(sa14.len() == 1);

        // partially out-of-range subslice is not empty, and has only up to valid items
        auto sa15 = a1.subslice(0, 4);
        REQUIRE(!sa15.is_empty());
        REQUIRE(sa15.len() == 2);
    }

    {
        auto a1 = nel::heaped::Vector<int>::empty();
        a1.push_back(1);
        a1.push_back(2);
        auto const c1 = std::move(a1);

        auto sc1 = c1.subslice(0, 0);
        REQUIRE(sc1.is_empty());

        // sub slice of non-empty array is not empty.
        auto sc2 = c1.subslice(0, 1);
        REQUIRE(!sc2.is_empty());
        REQUIRE(sc2.len() == 1);

        auto sc3 = c1.subslice(3, 4);
        REQUIRE(sc3.is_empty());

        auto sc4 = c1.subslice(1, 4);
        REQUIRE(!sc4.is_empty());
        REQUIRE(sc4.len() == 1);

        auto sc5 = c1.subslice(0, 4);
        REQUIRE(!sc5.is_empty());
        REQUIRE(sc5.len() == 2);
    }
}

TEST_CASE("heaped::Vector::iter()", "[heaped][Vector]")
{
    {
        // can create iter on empty vectors.
        auto a1 = nel::heaped::Vector<int>::empty();
        auto it1 = a1.iter();
        // and the iter is empty.
        REQUIRE(it1.next().is_none());

        auto const c1 = nel::heaped::Vector<int>::empty();
        auto itc1 = c1.iter();
        // and the iter is empty.
        REQUIRE(itc1.next().is_none());
    }

    {
        // can create iter on non empty vectors.
        auto a2 = nel::heaped::Vector<int>::fill(2, 2);
        auto it2 = a2.iter();
        REQUIRE(it2.next().unwrap() == 2);
        REQUIRE(it2.next().unwrap() == 2);
        REQUIRE(it2.next().is_none());

        auto const c2 = nel::heaped::Vector<int>::fill(2, 2);
        auto itc2 = c2.iter();
        REQUIRE(itc2.next().unwrap() == 2);
        REQUIRE(itc2.next().unwrap() == 2);
        REQUIRE(itc2.next().is_none());
    }
}

TEST_CASE("heaped::Vector::try_get", "[heaped][vector]")
{
    {
        // get on an empty vec gets nothing.
        auto a1 = nel::heaped::Vector<int>::empty();
        auto sa1 = a1.try_get(0);
        REQUIRE(sa1.is_none());

        auto const c1 = nel::heaped::Vector<int>::empty();
        auto sc1 = c1.try_get(0);
        REQUIRE(sc1.is_none());
    }

    {
        // in-range get on non-empty vec is not empty.
        auto a2 = nel::heaped::Vector<int>::fill(2, 3);
        auto sa2 = a2.try_get(0);
        REQUIRE(sa2.is_some());
        REQUIRE(sa2.unwrap() == 2);

        auto const c2 = nel::heaped::Vector<int>::fill(2, 3);
        auto sc2 = c2.try_get(0);
        REQUIRE(sc2.is_some());
        REQUIRE(sc2.unwrap() == 2);
    }

    {
        // out-of-range get on non-empty vec must return none.
        auto a2 = nel::heaped::Vector<int>::fill(2, 3);
        auto sa2 = a2.try_get(3);
        REQUIRE(sa2.is_none());

        auto const c2 = nel::heaped::Vector<int>::fill(2, 3);
        auto sc2 = c2.try_get(3);
        REQUIRE(sc2.is_none());
    }
}