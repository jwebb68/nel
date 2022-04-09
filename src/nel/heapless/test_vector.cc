#include <nel/heapless/vector.hh>

#include <catch2/catch.hpp>

// TODO: what of size/cap == 0, i//e Vector<T, 0> ?

TEST_CASE("heapless::Vector::empty", "[heapless][vector]")
{
    // must be able to create an empty vector..
    auto a1 = nel::heapless::Vector<int, 3>::empty();
    auto const c1 = nel::heapless::Vector<int, 3>::empty();

    // // empty vector must be empty
    // REQUIRE(a1.is_empty());
    // // empty vector must have len of 0
    // REQUIRE(a1.len() == 0);
}

#if 0
// heapless capacity is fixed..
TEST_CASE("heapless::Vector::with_capacity", "[heapless][vector]")
{
    {
        // must be able to create a vector with 0 cap..
        auto a1 = nel::heapless::Vector<int, 3>::with_capacity(0);
        auto const c1 = nel::heapless::Vector<int, 3>::with_capacity(0);
    }

    {
        // must be able to create a vector with !0 cap
        auto a2 = nel::heapless::Vector<int, 3>::with_capacity(10);
        auto const c2 = nel::heapless::Vector<int, 3>::with_capacity(10);
    }
    // // empty vector must be empty
    // REQUIRE(a1.is_empty());
    // // empty vector must have len of 0
    // REQUIRE(a1.len() == 0);
}
#endif

// how to test that Vector cannot be copied?
// it will fail at compile time.

TEST_CASE("heapless::Vector::move", "[heapless][vector]")
{
    {
        // empty Vector can be moved
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        auto a2 = std::move(a1);
        REQUIRE(a1.is_empty());
        REQUIRE(a2.is_empty());
    }

    {
        // not empty vector can be moved
        auto a3 = nel::heapless::Vector<int, 3>::empty();
        a3.push_back(2);

        auto a2 = std::move(a3);
        REQUIRE(!a2.is_empty());
        REQUIRE(a3.is_empty());
    }

    {
        // testing const Vector moving, but should fail at compile time.
        // auto const c1 = nel::heapless::Vector<int, 3>::empty();
        // auto const c2 = nel::heapless::Vector<int, 3>::fill(2,1);
        // c2 = std::move(c1);
    }
}

TEST_CASE("heapless::Vector::is_empty", "[heapless][vector]")
{
    {
        // empty vector must be empty
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        REQUIRE(a1.is_empty());

        auto const c1 = nel::heapless::Vector<int, 3>::empty();
        REQUIRE(c1.is_empty());
    }

    {
        // vector filled to length >0 must not be empty
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.push_back(1);
        REQUIRE(!a1.is_empty());
    }
}

TEST_CASE("heapless::Vector::capacity", "[heapless][Vector]")
{
    // heapless vector has fixed capacity
    auto a1 = nel::heapless::Vector<int, 3>::empty();
    REQUIRE(a1.capacity() == 3);

    auto const a2 = nel::heapless::Vector<int, 3>::empty();
    REQUIRE(a2.capacity() == 3);
}

TEST_CASE("heapless::Vector::len", "[heapless][Vector]")
{
    // empty heapless vector have 0 len
    {
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        REQUIRE(a1.len() == 0);

        auto const ca1 = nel::heapless::Vector<int, 3>::empty();
        REQUIRE(ca1.len() == 0);
    }

    {
        // vector filled to length >0 must have that length
        auto a2 = nel::heapless::Vector<int, 3>::empty();
        a2.push_back(1);
        REQUIRE(a2.len() == 1);

        a2.push_back(2);
        REQUIRE(a2.len() == 2);

        a2.push_back(3);
        REQUIRE(a2.len() == 3);

        // maxed out vector does not increase len
        a2.push_back(4);
        REQUIRE(a2.len() == 3);

        a2.pop_back();
        REQUIRE(a2.len() == 2);

        // cannot push_back to ao const vector...
    }
}

TEST_CASE("heapless::Vector::clear", "[heapless][Vector]")
{
    {
        // empty vector can be cleared
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.clear();
        REQUIRE(a1.is_empty());

        // const vecs cannot be cleared..
        // auto const c1 = nel::heapless::Vector<int, 3>::empty();
        // c1.clear();
        // REQUIRE(c1.is_empty());
    }

    {
        // vector filled to length >0 must have len of length
        auto a3 = nel::heapless::Vector<int, 3>::empty();
        a3.push_back(1);
        a3.clear();
        REQUIRE(a3.is_empty());

        // auto const c3 = nel::heapless::Vector<int, 3>::fill(2, 1);
        // cap = c3.capacity();
        // c3.clear();
        // REQUIRE(c3.is_empty());
        // REQUIRE(c3.capacity() == cap);

        // must also test that clearing calls destructor on any elements in vector.
    }
}

#if 0
TEST_CASE("heapless::Vector::reserve", "[heapless][Vector]")
{
    // heapless vecs always fail the reserve..?
    // reserve of N succeeds?
    // reserve of any other value fails?
}
#endif

// TODO Array::try_get(index).

// should this be .into_slice()?
// is this a conversion, or an as.
TEST_CASE("heapless::Vector::slice()", "[heapless][Vector]")
{
    {
        // full slice of empty vector is empty.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        auto sa1 = a1.slice();
        REQUIRE(sa1.is_empty());

        auto const c1 = nel::heapless::Vector<int, 3>::empty();
        auto sc1 = c1.slice();
        REQUIRE(sc1.is_empty());
    }

    {
        // full slice of non-empty vector is not empty.
        auto a2 = nel::heapless::Vector<int, 3>::empty();
        a2.push_back(1);
        auto sa2 = a2.slice();
        REQUIRE(!sa2.is_empty());
        REQUIRE(sa2.len() == 1);

        auto const ca2 = std::move(a2);
        auto sca2 = ca2.slice();
        REQUIRE(!sca2.is_empty());
        REQUIRE(sca2.len() == 1);
    }
}

TEST_CASE("heapless::Vector::iter()", "[heapless][Vector]")
{
    {
        // can create iter on empty vectors.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        auto it1 = a1.iter();
        // and the iter is empty.
        REQUIRE(it1.next().is_none());

        auto const c1 = nel::heapless::Vector<int, 3>::empty();
        auto itc1 = c1.iter();
        // and the iter is empty.
        REQUIRE(itc1.next().is_none());
    }

    {
        // can create iter on non empty vectors.
        auto a2 = nel::heapless::Vector<int, 3>::empty();
        a2.push_back(2);
        a2.push_back(2);

        auto it21 = a2.iter();
        REQUIRE(it21.next().unwrap() == 2);
        REQUIRE(it21.next().unwrap() == 2);
        REQUIRE(it21.next().is_none());

        // iter does not consume vec
        auto it22 = a2.iter();
        REQUIRE(it22.next().unwrap() == 2);
        REQUIRE(it22.next().unwrap() == 2);
        REQUIRE(it22.next().is_none());

        // how to get a non-mut iter (returning const refs) on a mut vec...?
        // // can get mut iter (returns mur refs)
        // auto it23 = a2.iter_mut();
        // REQUIRE(it23.next().unwrap() == 2);
        // REQUIRE(it23.next().unwrap() == 2);
        // REQUIRE(it23.next().is_none());
    }
}
