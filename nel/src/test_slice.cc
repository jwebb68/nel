#include "slice.hh"

#include <catch2/catch.hpp>
#include <iostream>

#define UNUSED(v) ((void)(v))


TEST_CASE("Slice::empty()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    REQUIRE(s1.is_empty());

    auto const c1 = nel::Slice<int>::empty();
    REQUIRE(c1.is_empty());

}


TEST_CASE("Slice::from(ptr,len)", "[slice]")
{
    int a1[] = {3, 1, 2, 4};
    auto s1 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    REQUIRE(s1.len() == 4);

    auto const c1 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    REQUIRE(c1.len() == 4);

}


TEST_CASE("Slice::is_empty()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    REQUIRE(s1.is_empty());

    auto const c1 = nel::Slice<int>::empty();
    REQUIRE(c1.is_empty());


    int a1[] = {3, 1, 2, 4};
    auto s2 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    REQUIRE(!s2.is_empty());

    auto const c2 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    REQUIRE(!c2.is_empty());

}


TEST_CASE("Slice::len()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    REQUIRE(s1.len() == 0);

    auto const c1 = nel::Slice<int>::empty();
    REQUIRE(c1.len() == 0);


    int a1[] = {3, 1, 2, 4};
    auto s2 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    REQUIRE(s2.len() == 4);

    auto const c2 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    REQUIRE(c2.len() == 4);
}


TEST_CASE("Slice::fill()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    s1.fill(2);

    // must fail to compile.
    // auto const c1 = nel::Slice<int>::empty();
    // c1.fill(2);


    int a1[] = {3, 1, 2, 4};
    auto s2 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    s2.fill(2);
    REQUIRE(a1[0] == 2);
    REQUIRE(a1[1] == 2);
    REQUIRE(a1[2] == 2);
    REQUIRE(a1[3] == 2);
}


TEST_CASE("Slice::iter()", "[slice]")
{
    auto s1 = nel::Slice<int>::empty();
    auto is1 = s1.iter();
    REQUIRE(is1.next().is_none());

    auto const c1 = nel::Slice<int>::empty();
    auto ic1 = c1.iter();
    REQUIRE(ic1.next().is_none());


    int a1[] = {3, 1, 2, 4};
    auto s2 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    auto is2 = s2.iter();

    REQUIRE(is2.next().unwrap() == 3);
    REQUIRE(is2.next().unwrap() == 1);
    REQUIRE(is2.next().unwrap() == 2);
    REQUIRE(is2.next().unwrap() == 4);
    REQUIRE(is2.next().is_none());

    auto const c2 = nel::Slice<int>::from(a1, sizeof(a1)/sizeof(a1[0]));
    auto ic2 = c2.iter();

    REQUIRE(ic2.next().unwrap() == 3);
    REQUIRE(ic2.next().unwrap() == 1);
    REQUIRE(ic2.next().unwrap() == 2);
    REQUIRE(ic2.next().unwrap() == 4);
    REQUIRE(ic2.next().is_none());

    auto is3 = s2.iter();
    is3.next().unwrap() = 5;
    auto is4 = s2.iter();
    REQUIRE(is4.next().unwrap() == 5);

}
