// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "iterator.hh"

#include <nel/heapless/array.hh>

#include <catch2/catch.hpp>

namespace nel
{
namespace test
{
namespace iterator
{

TEST_CASE("iterator::for_each", "[iterator]")
{
    {
        // for_each on the iter traits works.
        // auto a1 = nel::heapless::Array<int, 2>::try_from({7, 7}).unwrap();
        auto a1 = nel::heapless::Array<int, 2>::filled_with(7);
        a1.iter().for_each([](int &e) { e *= 3; });
    }
}

#if defined(RUST_LIKE) && defined(C_LIKE)
TEST_CASE("iterator::for_each2", "[iterator]")
{
    {
        // for_each on the iter traits works.
        // auto a1 = nel::heapless::Array<int, 2>::try_from({7, 7}).unwrap();
        auto a1 = nel::heapless::Array<int, 2>::filled_with(7);
        a1.iter().for_each2([](int &e) { e *= 3; });
    }
}
#endif

TEST_CASE("iterator::fold", "[iterator]")
{
    {
        // fold on the iter traits works.
        // auto a1 = nel::heapless::Array<int, 2>::try_from({7, 7}).unwrap();
        auto a1 = nel::heapless::Array<int, 2>::filled_with(7);
        // int folded
        //     = a1.iter().fold(0.0, std::function([](double &acc, int const &e) { acc += e; }));
        int folded = a1.iter().fold(0.0, [](double &acc, int const &e) { acc += e; });
        REQUIRE(folded == 14);
    }
}

TEST_CASE("iterator::first_n", "[iterator]")
{
#if defined(RUST_LIKE)
    {
        // first_n on the iter traits works.
        // auto a1 = nel::heapless::Array<int, 5>::try_from({7, 7, 7, 7, 7}).unwrap();
        auto a1 = nel::heapless::Array<int, 5>::filled_with(7);
        // auto a1 = nel::heapless::Array<int, 5>::filled_with(7,5);
        auto it1 = a1.iter().first_n(2);
        REQUIRE(it1.next().unwrap() == 7);
        REQUIRE(it1.next().unwrap() == 7);
        REQUIRE(it1.next().is_none());
    }
#endif
}

TEST_CASE("iterator::map", "[iterator]")
{
#if defined(RUST_LIKE)
    {
        // map function to each item in iter
        // auto a1 = nel::heapless::Array<int, 5>::try_from({7, 7, 7, 7, 7}).unwrap();
        auto a1 = nel::heapless::Array<int, 5>::filled_with(7);
        // auto a1 = nel::heapless::Array<int, 5>::filled_with(7,5);
        // auto it1 = a1.iter().map(std::function([](int &e) -> float { return e * 2; }));
        auto it1 = a1.iter().map<float>([](int &e) -> float { return e * 2; });
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().is_none());
    }
#endif
#if defined(C_LIKE)
    {
        // map function to each item in iter
        // auto a1 = nel::heapless::Array<int, 5>::try_from({7, 7, 7, 7, 7}).unwrap();
        auto a1 = nel::heapless::Array<int, 5>::filled_with(7);
        // auto a1 = nel::heapless::Array<int, 5>::filled_with(7,5);
        // auto it1 = a1.iter().map(std::function([](int &e) -> float { return e * 2; }));
        auto it1 = a1.iter().map<float>([](int &e) -> float { return e * 2; });

        REQUIRE(it1);
        REQUIRE(*it1 == 14.0);
        ++it1;
        REQUIRE(it1);
        REQUIRE(*it1 == 14.0);
        ++it1;
        REQUIRE(it1);
        REQUIRE(*it1 == 14.0);
        ++it1;
        REQUIRE(it1);
        REQUIRE(*it1 == 14.0);
        ++it1;
        REQUIRE(it1);
        REQUIRE(*it1 == 14.0);
        ++it1;
        REQUIRE(!it1);
    }
#endif
}

} // namespace iterator
} // namespace test
} // namespace nel
