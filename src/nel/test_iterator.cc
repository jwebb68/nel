#include "iterator.hh"

#include <nel/heapless/array.hh>

#include <catch2/catch.hpp>

TEST_CASE("iterator::for_each", "[iterator]")
{
    {
        // for_each on the iter traits works.
        auto a1 = nel::heapless::Array<int, 2>::try_from({7, 7}).unwrap();
        a1.iter().for_each([](int &e) { e *= 3; });
    }
}

TEST_CASE("iterator::fold", "[iterator]")
{
    {
        // fold on the iter traits works.
        auto a1 = nel::heapless::Array<int, 2>::try_from({7, 7}).unwrap();
        int folded
            = a1.iter().fold(0.0, std::function([](double &acc, int const &e) { acc += e; }));
        REQUIRE(folded == 14);
    }
}

TEST_CASE("iterator::first_n", "[iterator]")
{
    {
        // first_n on the iter traits works.
        auto a1 = nel::heapless::Array<int, 5>::try_from({7, 7, 7, 7, 7}).unwrap();
        auto it1 = a1.iter().first_n(2);
        REQUIRE(it1.next().unwrap() == 7);
        REQUIRE(it1.next().unwrap() == 7);
        REQUIRE(it1.next().is_none());
    }
}

TEST_CASE("iterator::map", "[iterator]")
{
    {
        // map function to each item in iter
        auto a1 = nel::heapless::Array<int, 5>::try_from({7, 7, 7, 7, 7}).unwrap();
        auto it1 = a1.iter().map(std::function([](int &e) -> float { return e * 2; }));
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().unwrap() == 14.0);
        REQUIRE(it1.next().is_none());
    }
}
