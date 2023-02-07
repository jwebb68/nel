// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <catch2/catch.hpp>

#include <nel/heapless/queue.hh>
#include <nel/memory.hh> // nel::move()
#include <nel/defs.hh>

TEST_CASE("heapless:Queue::create", "[heapless][queue]")
{
    // must be able to create an empty queue..
    auto a1 = nel::heapless::Queue<int, 3>::empty();

    // dunno why the need to create a const queue...
    auto const c1 = nel::heapless::Queue<int, 3>::empty();
}

// how to test that array cannot be copied?
// it will fail at compile time.
TEST_CASE("heapless::Queue::move", "[heapless][queue]")
{
    {
        // empty array can be moved to another location
        auto a2 = nel::heapless::Queue<int, 3>::empty();
        auto a1 = nel::heapless::Queue<int, 3>::empty();
        a2 = nel::move(a1);
        // how to test?
    }
    {
        // non-empty queue can be moved
        auto a3 = nel::heapless::Queue<int, 3>::empty();
        auto a4 = nel::heapless::Queue<int, 3>::empty();
        a4.push(0).is_ok();
        a4.push(1).is_ok();

        a3 = nel::move(a4);

        REQUIRE(a4.len() == 0);
        REQUIRE(a3.len() == 2);
        // order is preserved..
        REQUIRE(a3.pop().unwrap() == 0);
        REQUIRE(a3.pop().unwrap() == 1);
    }

    // cannot move const queues.. must be a compile fail..how to test
}

TEST_CASE("heapless::Queue::is_empty", "[heapless][queue]")
{
    {
        // empty queue must be empty
        auto a1 = nel::heapless::Queue<int, 3>::empty();
        REQUIRE(a1.is_empty());

        auto const c1 = nel::heapless::Queue<int, 3>::empty();
        REQUIRE(c1.is_empty());
    }

    {
        // non-empty queue is not empty
        auto a2 = nel::heapless::Queue<int, 3>::empty();
        a2.push(1).is_ok();
        REQUIRE(!a2.is_empty());

        // popting last value makes queue empty again.
        auto v1 = a2.pop().unwrap();
        REQUIRE(a2.is_empty());
        NEL_UNUSED(v1);
    }

    // const queue cannot be push to.. copile fail.. how to test..is_ok().
}

TEST_CASE("heapless::Queue::len", "[heapless][queue]")
{
    {
        // empty queue must have len 0
        auto a1 = nel::heapless::Queue<int, 3>::empty();
        REQUIRE(a1.len() == 0);

        // auto const c1 = nel::heapless::Array<int, 3>::empty();
        // REQUIRE(c1.len() == 0);
    }

    {
        // queue filled to length >0 must have len of length
        auto a3 = nel::heapless::Queue<int, 3>::empty();
        a3.push(1).is_ok();
        REQUIRE(a3.len() == 1);

        a3.push(2).is_ok();
        REQUIRE(a3.len() == 2);

        a3.push(3).is_ok();
        REQUIRE(a3.len() == 3);

        // maxed out queue cannot grow in size.
        a3.push(4).is_ok();
        REQUIRE(a3.len() == 3);
    }
}

// should this be .into_slice()?
// is this a conversion, or an as.
TEST_CASE("heapless::Queue::push()", "[heapless][queue]")
{
    {
        auto a1 = nel::heapless::Queue<int, 3>::empty();

        // empty queue can be push to  (what if size == 0).is_ok()?
        auto p0 = a1.push(1);
        REQUIRE(p0.is_ok());

        // maxing out queue must work.
        a1.push(2).is_ok();
        auto p1 = a1.push(3);
        REQUIRE(p1.is_ok());

#if 0
        // over-doing it must fail
        auto p3 = a1.push(4);
        REQUIRE(p3.is_err());
#else
        // over-doing it must not fail
        auto p3 = a1.push(4);
        REQUIRE(p3.is_ok());
#endif

        // removing items allows more pushing
        auto g4 = a1.pop();
        auto p4 = a1.push(5);
        REQUIRE(p4.is_ok());
    }

    {
        // and pushing then popping preserves order
        auto a2 = nel::heapless::Queue<int, 3>::empty();
        a2.push(1).is_ok();
        a2.push(2).is_ok();
        a2.push(3).is_ok();

        REQUIRE(a2.pop().unwrap() == 1);
        REQUIRE(a2.pop().unwrap() == 2);
        REQUIRE(a2.pop().unwrap() == 3);
    }
}

TEST_CASE("heapless::Queue::pop()", "[heapless][queue]")
{
    auto a1 = nel::heapless::Queue<int, 3>::empty();

    // empty queue cannot be got from  (what if size == 0)?
    auto g0 = a1.pop();
    REQUIRE(g0.is_none());

    // pushing value into queue can be got.
    auto r1 = a1.push(1);
    auto g1 = a1.pop();
    REQUIRE(g1.is_some());
    REQUIRE(g1.unwrap() == 1);
}
