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

        bool valid;
        int val;

        ~Stub()
        {
            if (valid) { instances -= 1; }
        }

        // array needs default ctor..
        Stub()
            : Stub(10)
        {
        }

        Stub(int v)
            : valid(true)
            , val(v)
        {
            instances += 1;
        }

        Stub(Stub &&o)
            : valid(nel::move(o.valid))
            , val(nel::move(o.val))
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

        // array filling needs to copy
        Stub(Stub const &o) = default;
        Stub &operator=(Stub const &o) = default;
};

int Stub::instances = 0;
int Stub::move_ctor = 0;
int Stub::move_assn = 0;

TEST_CASE("heapless::Array: dtor deletes contained", "[heapless][array]")
{
    Stub::reset();
    {
        nel::heapless::Array<Stub, 5> arr;
        // array, all elements are initialised.
        REQUIRE(Stub::instances == 5);
    }
    // array: should be dtructed when going out of scope.
    // array must delete all instances contained.
    REQUIRE(Stub::instances == 0);
}

TEST_CASE("heapless::Array: move-ctor invalidates src", "[heapless][array]")
{
    Stub::reset();
    auto arr1 = nel::heapless::Array<Stub, 5>();
    auto arr2 = nel::move(arr1);

    // all elements of src must be invalidated by move.
#if 0
    auto arr1_all_invalid
        = !arr1.iter().fold(false, [](bool &acc, auto const &e) { acc = acc || e.valid; });
#else
    auto arr1_all_invalid
        = !arr1.iter().fold(false, [](bool const &acc, auto const &e) { return acc || e.valid; });
#endif
    REQUIRE(arr1_all_invalid);

    // all elements of dst must be valid.
#if 0
    auto arr2_all_valid
        = arr2.iter().fold(true, [](bool &acc, auto const &e) { acc = acc && e.valid; });
#else
    auto arr2_all_valid
        = arr2.iter().fold(true, [](bool const &acc, auto const &e) { return acc && e.valid; });
#endif
    REQUIRE(arr2_all_valid);

    // move ctor was used not move assn
    REQUIRE(Stub::move_ctor == 5);
    REQUIRE(Stub::move_assn == 0);
}

TEST_CASE("heapless::Array: move-assn invalidates src", "[heapless][array]")
{
    Stub::reset();
    auto arr1 = nel::heapless::Array<Stub, 5>();
    auto arr2 = nel::heapless::Array<Stub, 5>();
    arr2 = nel::move(arr1);

    // all elements of src must be invalidated by move.
    // must call move-assgn oper
#if 0
    auto arr1_all_invalid
        = !arr1.iter().fold(false, [](bool &acc, auto const &e) { acc = acc || e.valid; });
#else
    auto arr1_all_invalid
        = !arr1.iter().fold(false, [](bool const &acc, auto const &e) { return acc || e.valid; });
#endif
    REQUIRE(arr1_all_invalid);

    // all elements of dst must be valid.
#if 0
    auto arr2_all_valid
        = arr2.iter().fold(true, [](bool &acc, auto const &e) { acc = acc && e.valid; });
#else
    auto arr2_all_valid
        = arr2.iter().fold(true, [](bool const &acc, auto const &e) { return acc && e.valid; });
#endif
    REQUIRE(arr2_all_valid);

    // move assgn was used not move ctor
    REQUIRE(Stub::move_ctor == 0);
    REQUIRE(Stub::move_assn == 5);
}

TEST_CASE("heapless::Array::move", "[heapless][array]")
{
    {
        // array of pods can be move-ctord
        auto a1 = nel::heapless::Array<int, 3>::filled_with(3);
        auto a2 = nel::move(a1);

        // moved dst must contain src values..
#if 0
        auto r = a2.iter().fold(true, [](bool &acc, int const &v) { acc = acc && (v == 3); });
#else
        auto r = a2.iter().fold(true, [](bool const &acc, int const &v) { return acc && (v == 3); });
#endif
        REQUIRE(r);
        // moved src must be invalidated .. cannot check with pods.
    }

    {
        // array of pods can be move-assgnd
        auto a2 = nel::heapless::Array<int, 3>::filled_with(3);
        auto a1 = nel::heapless::Array<int, 3>::filled_with(1);
        a2 = nel::move(a1);

        // moved dst must contain src values..
#if 0
        auto r = a2.iter().fold(true, [](bool &acc, int const &v) { acc = acc && (v == 1); });
#else
        auto r = a2.iter().fold(true, [](bool const &acc, int const &v) { return acc && (v == 1); });
#endif
        REQUIRE(r);
        // moved src must be invalidated .. cannot check with pods.
    }
}

// how to test that array cannot be copied?
// it will fail at compile time.

TEST_CASE("heapless::Array::filled_with", "[heapless][array]")
{
    {
        // must fill array with same value
        auto a1 = nel::heapless::Array<int, 3>::filled_with(5);
        REQUIRE(a1.try_get(0).unwrap() == 5);
        REQUIRE(a1.try_get(1).unwrap() == 5);
        REQUIRE(a1.try_get(2).unwrap() == 5);
    }

    {
        auto const c1 = nel::heapless::Array<int, 3>::filled_with(5);
        REQUIRE(c1.try_get(0).unwrap() == 5);
        REQUIRE(c1.try_get(1).unwrap() == 5);
        REQUIRE(c1.try_get(2).unwrap() == 5);
    }

    // and if using udt
    // must fill with copies of udt.
    // maybe should be try_filled_with (as copy can fail).
    // should filled_with auto convert to T? or be explicit.
    {
        auto a2 = nel::heapless::Array<Stub, 3>::filled_with(Stub(1000));
        REQUIRE(a2.try_get(0).unwrap().val == 1000);
        REQUIRE(a2.try_get(1).unwrap().val == 1000);
        REQUIRE(a2.try_get(2).unwrap().val == 1000);
    }

    {
        auto c2 = nel::heapless::Array<Stub, 3>::filled_with(Stub(1000));
        REQUIRE(c2.try_get(0).unwrap().val == 1000);
        REQUIRE(c2.try_get(1).unwrap().val == 1000);
        REQUIRE(c2.try_get(2).unwrap().val == 1000);
    }
}

// test that init list works..
// test with pod (int/bool etc)
// test with pod narrowing (give int, narrow to int8)
// test with widening (give int8, store int)
// test compile fail with unsigned->signed conversion (give int, store uint; give uint, store int)
// test udt default.
// test udt
TEST_CASE("heapless::Array::initlist-ctor", "[heapless][array]")
{
    // these tests are compile tests, if compiles then is good.

    {
        // pod: can create using empty list
        auto a1 = nel::heapless::Array<int, 3> {};
        nel::unused(a1);

        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<int, 3> a3 = {};
        nel::heapless::Array<int, 3> a4 {};
        nel::unused(a3);
        nel::unused(a4);

        auto const c1 = nel::heapless::Array<int, 3> {};
        nel::unused(c1);
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<int, 3> const c3 = {};
        nel::heapless::Array<int, 3> const c4 {};
        nel::unused(c3);
        nel::unused(c4);
    }

    {
        // pod: can create using full list
        auto a1 = nel::heapless::Array<int, 3> {1, 2, 3};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<int, 3> a3 = {5, 6, 7};
        nel::heapless::Array<int, 3> a4 {8, 9, 10};

        auto const c1 = nel::heapless::Array<int, 3> {2, 3, 4};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<int, 3> const c3 = {5, 6, 7};
        nel::heapless::Array<int, 3> const c4 {8, 9, 10};
    }

    {
        // pod: can create using partial list
        auto a1 = nel::heapless::Array<int, 3> {1, 2};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<int, 3> a3 = {5, 6};
        nel::heapless::Array<int, 3> a4 {8, 9};

        auto const c1 = nel::heapless::Array<int, 3> {2, 3};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<int, 3> const c3 = {5, 6};
        nel::heapless::Array<int, 3> const c4 {8, 9};
    }

    {
        // pod: can create using full list, narrowing
        auto a1 = nel::heapless::Array<int8_t, 3> {1, 2, 3};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<int8_t, 3> a3 = {5, 6, 7};
        nel::heapless::Array<int8_t, 3> a4 {8, 9, 10};

        auto const c1 = nel::heapless::Array<int8_t, 3> {2, 3, 4};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int8_t, 3>({});
        nel::heapless::Array<int8_t, 3> const c3 = {5, 6, 7};
        nel::heapless::Array<int8_t, 3> const c4 {8, 9, 10};
    }

    {
        // pod: can create using full list, expanding
        auto a1 = nel::heapless::Array<long int, 3> {1, 2, 3};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<long int, 3> a3 = {5, 6, 7};
        nel::heapless::Array<long int, 3> a4 {8, 9, 10};

        auto const c1 = nel::heapless::Array<long int, 3> {2, 3, 4};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<long int, 3>({});
        nel::heapless::Array<long int, 3> const c3 = {5, 6, 7};
        nel::heapless::Array<long int, 3> const c4 {8, 9, 10};
    }

    //---------------------------------------------------------------
    {
        // udt: can create using full list, implicit ctor
        auto a1 = nel::heapless::Array<Stub, 3> {1, 2, 3};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<int, 3>({});
        nel::heapless::Array<Stub, 3> a3 = {5, 6, 7};
        nel::heapless::Array<Stub, 3> a4 {8, 9, 10};

        auto const c1 = nel::heapless::Array<Stub, 3> {2, 3, 4};
        // wants copy ctor..
        // auto a2 = nel::heapless::Array<long int, 3>({});
        nel::heapless::Array<Stub, 3> const c3 = {5, 6, 7};
        nel::heapless::Array<Stub, 3> const c4 {8, 9, 10};
    }
}

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

TEST_CASE("heapless::Array::is_empty", "[heapless][array]")
{
    // Arrays cannot be empty..
    // cannot have 0 sized array in C++, compiler will refuse to compile.
    // auto a0 = nel::heapless::Array<int, 0>::filled_with(0);
    // REQUIRE(a0.is_empty());

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
#if defined(RUST_LIKE)
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
#endif

#if defined(C_LIKE)
    // can create iter on non empty arrays.
    // auto a2 = nel::heapless::Array<int, 3>::try_from({2, 2, 2}).unwrap();
    auto a3 = nel::heapless::Array<int, 3>::filled_with(2);
    auto it3 = a3.iter();
    REQUIRE(it3);
    REQUIRE(*it3 == 2);
    ++it3;
    REQUIRE(it3);
    REQUIRE(*it3 == 2);
    ++it3;
    REQUIRE(it3);
    REQUIRE(*it3 == 2);
    ++it3;
    REQUIRE(!it3);
#endif
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
