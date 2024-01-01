// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <nel/heapless/vector.hh>

#include <nel/memory.hh> // nel::move()

#include <catch2/catch.hpp>

// TODO: what of size/cap == 0, i//e Vector<T, 0> ?
namespace nel
{
namespace test
{
namespace heapless
{
namespace vector
{

struct Stub
{
        static int instances;
        static int move_ctor;
        static int move_assn;

        static void reset()
        {
            instances = 0;
            move_ctor = 0;
            move_assn = 0;
        }

        int val;
        bool valid;

        ~Stub()
        {
            if (valid) { instances -= 1; }
        }

        Stub(int v)
            : val(v)
            , valid(true)
        {
            instances += 1;
        }

        Stub(Stub &&o)
            : val(nel::move(o.val))
            , valid(nel::move(o.valid))
        {
            // move: moves a value, so does not create a new value.
            // move: moves a value, so does create a value in a new location.
            // remember, the dtor of o will be called..
            o.valid = false;
            move_ctor += 1;
        }

        Stub &operator=(Stub &&o)
        {
            // move: moves a value, so does not create a new value.
            // move: moves a value, so does create a value in a new location.
            val = nel::move(o.val);
            valid = nel::move(o.valid);
            o.valid = false;
            move_assn += 1;
            return *this;
        }

        Stub(Stub const &o) = delete;
        Stub &operator=(Stub const &o) = delete;

        Stub() = delete;
};

int Stub::instances = 0;
int Stub::move_ctor = 0;
int Stub::move_assn = 0;

TEST_CASE("heapless::Vector: dtor deletes contained", "[heapless][vector]")
{
    Stub::reset();
    {
        nel::heapless::Vector<Stub, 5> vec;
        vec.push(nel::move(Stub(1))).unwrap();
        vec.push(nel::move(Stub(2))).unwrap();

        // vec: all pushed elements are initialised.
        REQUIRE(Stub::instances == 2);
    }
    // vec: should be dtructed when going out of scope.
    // vec must delete all instances contained.
    REQUIRE(Stub::instances == 0);
}

TEST_CASE("heapless::Vector: move-ctor invalidates src", "[heapless][vector]")
{
    {
        // move empty/initial vector
        auto vec1 = nel::heapless::Vector<Stub, 5>();

        Stub::reset();
        auto vec2 = nel::move(vec1);

        REQUIRE(vec2.len() == 0);
        REQUIRE(vec1.len() == 0);
    }

    {
        // move partially filled vector
        auto vec1 = nel::heapless::Vector<Stub, 5>();
        vec1.push(nel::move(Stub(1))).unwrap();
        vec1.push(nel::move(Stub(2))).unwrap();

        Stub::reset();
        auto vec2 = nel::move(vec1);

        // T's was moved using it's move ops.
        // i.e. not using bitcopying.
        // note: will be moved as part of pushing + moved into push
        REQUIRE(Stub::move_ctor == 2);
        REQUIRE(Stub::move_assn == 0);

        // src len must now be 0, so contents are not dtucted.
        REQUIRE(vec1.len() == 0);

        REQUIRE(vec2.len() == 2);
        REQUIRE(vec2.unchecked_get(0).val == 1);
        REQUIRE(vec2.unchecked_get(1).val == 2);
    }
}

TEST_CASE("heapless::Vector: move-assn invalidates src", "[heapless][vector]")
{
    {
        // move empty/initial vector onto empty
        auto vec1 = nel::heapless::Vector<Stub, 5>();
        auto vec2 = nel::heapless::Vector<Stub, 5>();

        Stub::reset();
        vec2 = nel::move(vec1);

        REQUIRE(vec2.len() == 0);
        REQUIRE(vec1.len() == 0);
    }

    {
        // move partially filled vector onto empty
        auto vec1 = nel::heapless::Vector<Stub, 5>();
        vec1.push(Stub(1)).unwrap();
        vec1.push(Stub(2)).unwrap();

        auto vec2 = nel::heapless::Vector<Stub, 5>();

        Stub::reset();

        vec2 = nel::move(vec1);

        // T's was moved using it's move ops.
        // i.e. not using bitcopying.
        REQUIRE(Stub::move_ctor == 2);
        // vec move will not call move ssgn
        REQUIRE(Stub::move_assn == 0);

        // src len must now be 0, so contents are not dtucted.
        REQUIRE(vec1.len() == 0);

        REQUIRE(vec2.len() == 2);
        REQUIRE(vec2.unchecked_get(0).val == 1);
        REQUIRE(vec2.unchecked_get(1).val == 2);
    }

    {
        // move empty onto partially filled
        auto vec1 = nel::heapless::Vector<Stub, 5>();

        auto vec2 = nel::heapless::Vector<Stub, 5>();
        vec2.push(Stub(1)).unwrap();
        vec2.push(Stub(2)).unwrap();

        Stub::reset();

        vec2 = nel::move(vec1);

        // T's was moved using it's move ops.
        // i.e. not using bitcopying.
        REQUIRE(Stub::move_ctor == 0);
        // vec move will not call move assgn
        REQUIRE(Stub::move_assn == 0);
        // move must have destroyed the contained instances.
        // note: reset will reset count to 0 when 2 are in play.
        REQUIRE(Stub::instances == -2);

        // src len must now be 0, so contents are not dtucted.
        REQUIRE(vec1.len() == 0);

        REQUIRE(vec2.len() == 0);
    }
}

TEST_CASE("heapless::Vector::move", "[heapless][vector]")
{
    {
        // empty Vector can be moved
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        auto a2 = nel::move(a1);
        REQUIRE(a1.is_empty());
        REQUIRE(a2.is_empty());
    }

    {
        // not empty vector can be moved
        auto a3 = nel::heapless::Vector<int, 3>::empty();
        a3.push(2).is_ok();

        auto a2 = nel::move(a3);
        REQUIRE(!a2.is_empty());
        REQUIRE(a3.is_empty());
    }

    {
        // testing const Vector moving, but should fail at compile time.
        // auto const c1 = nel::heapless::Vector<int, 3>::empty();
        // auto const c2 = nel::heapless::Vector<int, 3>::fill(2,1);
        // c2 = nel::move(c1);
    }

    {
        // array of pods can be move-ctord
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.push(3).unwrap();
        a1.push(3).unwrap();
        a1.push(3).unwrap();

        auto a2 = nel::move(a1);

        // moved dst must contain src values..
        auto r = a2.iter().fold(true, [](bool &acc, int const &v) { acc = acc && (v == 3); });
        REQUIRE(r);
        // moved src must be invalidated .. cannot check with pods.
    }

    {
        // array of pods can be move-assgnd
        auto a2 = nel::heapless::Vector<int, 3>::empty();
        a2.push(3).unwrap();
        a2.push(3).unwrap();
        a2.push(3).unwrap();

        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.push(1).unwrap();
        a1.push(1).unwrap();

        a2 = nel::move(a1);

        REQUIRE(a2.len() == 2);
        // moved dst must contain src values..
        REQUIRE(a2.len() == 2);
        auto r = a2.iter().fold(true, [](bool &acc, int const &v) { acc = acc && (v == 1); });
        REQUIRE(r);
        // TODO: moved src must be invalidated .. cannot check with pods.
    }
}

// how to test that Vector cannot be copied?
// it will fail at compile time.

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
        a1.push(1).is_ok();
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
        a2.push(1).is_ok();
        REQUIRE(a2.len() == 1);

        a2.push(2).is_ok();
        REQUIRE(a2.len() == 2);

        a2.push(3).is_ok();
        REQUIRE(a2.len() == 3);

        // maxed out vector does not increase len
        a2.push(4).is_ok();
        REQUIRE(a2.len() == 3);

        a2.pop();
        REQUIRE(a2.len() == 2);

        // cannot push to ao const vector...is_ok().
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
        a3.push(1).is_ok();
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

TEST_CASE("heapless::Vector::resize", "[heapless][Vector]")
{
    {
        // resize empty to 0 does not change len
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.resize(0, 4);
        REQUIRE(a1.len() == 0);
    }

    {
        // resize empty to less than capacity adjusts len, fills in elements.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.resize(2, 4);
        REQUIRE(a1.len() == 2);
        REQUIRE(a1[0] == 4);
        REQUIRE(a1[1] == 4);
    }

    {
        // resize non-empty to less than capacity adjusts len, fills in elements.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.push(1).unwrap();
        REQUIRE(a1[0] == 1);

        a1.resize(2, 4);

        REQUIRE(a1.len() == 2);
        REQUIRE(a1[0] == 1);
        REQUIRE(a1[1] == 4);
    }

    {
        // resize empty to capacity adjusts len, fills in elements.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.resize(3, 4);
        REQUIRE(a1.len() == 3);
        REQUIRE(a1[0] == 4);
        REQUIRE(a1[1] == 4);
        REQUIRE(a1[2] == 4);
    }

    {
        // resize empty to over-capacity maxes out at capacity,
        // adjusts len, fills in elements.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.resize(4, 4);
        REQUIRE(a1.len() == 3);
        REQUIRE(a1[0] == 4);
        REQUIRE(a1[1] == 4);
        REQUIRE(a1[2] == 4);
    }

    {
        // resize non-empty to smaller adjusts len, drops elements.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.push(1).unwrap();
        a1.push(2).unwrap();
        REQUIRE(a1[0] == 1);
        REQUIRE(a1[1] == 2);

        a1.resize(1, 4);

        REQUIRE(a1.len() == 1);
        REQUIRE(a1[0] == 1);
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
        a2.push(1).is_ok();
        auto sa2 = a2.slice();
        REQUIRE(!sa2.is_empty());
        REQUIRE(sa2.len() == 1);

        auto const ca2 = nel::move(a2);
        auto sca2 = ca2.slice();
        REQUIRE(!sca2.is_empty());
        REQUIRE(sca2.len() == 1);
    }
}

TEST_CASE("heapless::Vector::slice(b,e)", "[heapless][vector]")
{
    {
        // sub slice of empty vec is empty.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        auto sa1 = a1.slice(0, 0);
        REQUIRE(sa1.is_empty());
        REQUIRE(sa1.len() == 0);

        auto sa2 = a1.slice(3, 5);
        REQUIRE(sa2.is_empty());
        REQUIRE(sa2.len() == 0);
    }

    {
        // in-range slice is not empty
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.push(1).is_ok();
        a1.push(2).is_ok();

        auto sa12 = a1.slice(0, 1);
        REQUIRE(!sa12.is_empty());
        REQUIRE(sa12.len() == 1);

        // out-of-range slice is empty
        auto sa13 = a1.slice(3, 4);
        REQUIRE(sa13.is_empty());

        // partially out-of-range slice is not empty, and has only up to valid items
        auto sa14 = a1.slice(1, 4);
        REQUIRE(!sa14.is_empty());
        REQUIRE(sa14.len() == 1);

        // partially out-of-range slice is not empty, and has only up to valid items
        auto sa15 = a1.slice(0, 4);
        REQUIRE(!sa15.is_empty());
        REQUIRE(sa15.len() == 2);
    }

    {
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        a1.push(1).is_ok();
        a1.push(2).is_ok();
        auto const c1 = nel::move(a1);

        auto sc1 = c1.slice(0, 0);
        REQUIRE(sc1.is_empty());

        // sub slice of non-empty array is not empty.
        auto sc2 = c1.slice(0, 1);
        REQUIRE(!sc2.is_empty());
        REQUIRE(sc2.len() == 1);

        auto sc3 = c1.slice(3, 4);
        REQUIRE(sc3.is_empty());

        auto sc4 = c1.slice(1, 4);
        REQUIRE(!sc4.is_empty());
        REQUIRE(sc4.len() == 1);

        auto sc5 = c1.slice(0, 4);
        REQUIRE(!sc5.is_empty());
        REQUIRE(sc5.len() == 2);
    }
}

TEST_CASE("heapless::Vector::iter()", "[heapless][Vector]")
{
#if defined(RUST_LIKE)
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
        a2.push(2).is_ok();
        a2.push(2).is_ok();

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
#endif
#if defined(C_LIKE)
    {
        // can create iter on empty vectors.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        auto it1 = a1.iter();
        // and the iter is empty.
        REQUIRE(!it1);

        auto const c1 = nel::heapless::Vector<int, 3>::empty();
        auto itc1 = c1.iter();
        // and the iter is empty.
        REQUIRE(!itc1);
    }

    {
        // can create iter on non empty vectors.
        auto a2 = nel::heapless::Vector<int, 3>::empty();
        a2.push(2).unwrap();
        a2.push(3).unwrap();

        auto it21 = a2.iter();

        REQUIRE(it21);
        REQUIRE(*it21 == 2);

        ++it21;
        REQUIRE(it21);
        REQUIRE(*it21 == 3);

        ++it21;
        REQUIRE(!it21);

        // iter does not consume vec
        auto it22 = a2.iter();
        REQUIRE(it22);
        REQUIRE(*it22 == 2);
        ++it22;
        REQUIRE(it22);
        REQUIRE(*it22 == 3);
        ++it22;
        REQUIRE(!it22);

        // how to get a non-mut iter (returning const refs) on a mut vec...?
        // // can get mut iter (returns mur refs)
        // auto it23 = a2.iter_mut();
        // REQUIRE(it23.next().unwrap() == 2);
        // REQUIRE(it23.next().unwrap() == 2);
        // REQUIRE(it23.next().is_none());
    }

#endif
}

TEST_CASE("heapless::Vector::try_get()", "[heapless][Vector]")
{
    {
        // get on empty vector is always none.
        auto a1 = nel::heapless::Vector<int, 3>::empty();
        auto sa1 = a1.try_get(0);
        REQUIRE(sa1.is_none());

        auto const c1 = nel::heapless::Vector<int, 3>::empty();
        auto sc1 = c1.try_get(0);
        REQUIRE(sc1.is_none());
    }

    {
        // in-range try_get of non-empty vector is ref to value.
        auto a2 = nel::heapless::Vector<int, 3>::empty();
        a2.push(1).is_ok();
        auto sa2 = a2.try_get(0);
        REQUIRE(sa2.is_some());
        REQUIRE(sa2.unwrap() == 1);

        auto const ca2 = nel::move(a2);
        auto sca2 = ca2.try_get(0);
        REQUIRE(sca2.is_some());
        REQUIRE(sca2.unwrap() == 1);
    }

    {
        // out-of-range try_get of non-empty vector is none.
        auto a2 = nel::heapless::Vector<int, 3>::empty();
        a2.push(1).is_ok();
        auto sa2 = a2.try_get(1);
        REQUIRE(sa2.is_none());

        auto const ca2 = nel::move(a2);
        auto sca2 = ca2.try_get(1);
        REQUIRE(sca2.is_none());
    }
}

} // namespace vector
} // namespace heapless
} // namespace test
} // namespace nel
