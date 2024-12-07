// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include <catch2/catch.hpp>

#include <nel/heapless/queue.hh>
#include <nel/memory.hh> // nel::move()
#include <nel/defs.hh>

namespace nel
{
namespace test
{
namespace heapless
{
namespace queue
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

TEST_CASE("heapless::Queue: dtor deletes contained", "[heapless][queue]")
{
    Stub::reset();
    {
        nel::heapless::Queue<Stub, 5> q;
        q.push(nel::move(Stub(1))).unwrap();
        q.push(nel::move(Stub(2))).unwrap();

        // queue: all pushed elements are initialised.
        REQUIRE(Stub::instances == 2);
    }
    // queue: should be dtructed when going out of scope.
    // queue must delete all instances contained.
    REQUIRE(Stub::instances == 0);
}

TEST_CASE("heapless::Queue: move-ctor invalidates src", "[heapless][queue]")
{
    {
        // move empty/initial queue
        auto q1 = nel::heapless::Queue<Stub, 5>();

        Stub::reset();
        auto q2 = nel::move(q1);

        REQUIRE(q2.len() == 0);
        REQUIRE(q1.len() == 0);
    }

    {
        // move partially filled queue
        auto q1 = nel::heapless::Queue<Stub, 5>();
        q1.push(nel::move(Stub(1))).unwrap();
        q1.push(nel::move(Stub(2))).unwrap();

        Stub::reset();
        auto q2 = nel::move(q1);

        // T's was moved using it's move ops.
        // i.e. not using bitcopying.
        // note: will be moved as part of pushing + moved into push
        REQUIRE(Stub::move_ctor == 2);
        REQUIRE(Stub::move_assn == 0);

        // src len must now be 0, so contents are not dtucted.
        REQUIRE(q1.len() == 0);

        REQUIRE(q2.len() == 2);
        REQUIRE(q2.pop().unwrap().val == 1);
        REQUIRE(q2.pop().unwrap().val == 2);
    }
}

TEST_CASE("heapless::Queue: move-assn invalidates src", "[heapless][queue]")
{
    {
        // move empty/initial vector onto empty
        auto q1 = nel::heapless::Queue<Stub, 5>();
        auto q2 = nel::heapless::Queue<Stub, 5>();

        Stub::reset();
        q2 = nel::move(q1);

        REQUIRE(q2.len() == 0);
        REQUIRE(q1.len() == 0);
    }

    {
        // move partially filled qtor onto empty
        auto q1 = nel::heapless::Queue<Stub, 5>();
        q1.push(Stub(1)).unwrap();
        q1.push(Stub(2)).unwrap();

        auto q2 = nel::heapless::Queue<Stub, 5>();

        Stub::reset();

        q2 = nel::move(q1);

        // T's was moved using it's move ops.
        // i.e. not using bitcopying.
        REQUIRE(Stub::move_ctor == 2);
        // q move will not call move ssgn
        REQUIRE(Stub::move_assn == 0);

        // src len must now be 0, so contents are not dtucted.
        REQUIRE(q1.len() == 0);

        REQUIRE(q2.len() == 2);
        REQUIRE(q2.pop().unwrap().val == 1);
        REQUIRE(q2.pop().unwrap().val == 2);
    }

    {
        // move empty onto partially filled
        auto q1 = nel::heapless::Queue<Stub, 5>();

        auto q2 = nel::heapless::Queue<Stub, 5>();
        q2.push(Stub(1)).unwrap();
        q2.push(Stub(2)).unwrap();

        Stub::reset();

        q2 = nel::move(q1);

        // T's was moved using it's move ops.
        // i.e. not using bitcopying.
        REQUIRE(Stub::move_ctor == 0);
        // q move will not call move assgn
        REQUIRE(Stub::move_assn == 0);
        // move must have destroyed the contained instances.
        // note: reset will reset count to 0 when 2 are in play.
        REQUIRE(Stub::instances == -2);

        // src len must now be 0, so contents are not dtucted.
        REQUIRE(q1.len() == 0);

        REQUIRE(q2.len() == 0);
    }
}

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
        a4.push(0).unwrap();
        a4.push(1).unwrap();

        a3 = nel::move(a4);

        REQUIRE(a4.len() == 0);
        REQUIRE(a3.len() == 2);
        // order is preserved..
        REQUIRE(a3.pop().unwrap() == 0);
        REQUIRE(a3.pop().unwrap() == 1);
    }

    // cannot move const queues.. must be a compile fail..how to test?
}

TEST_CASE("heapless:Queue::create", "[heapless][queue]")
{
    // must be able to create an empty queue..
    auto a1 = nel::heapless::Queue<int, 3>::empty();

    // dunno why the need to create a const queue...
    auto const c1 = nel::heapless::Queue<int, 3>::empty();
}

// how to test that array cannot be copied?
// it will fail at compile time.

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
        a2.push(1).unwrap();
        REQUIRE(!a2.is_empty());

        // popting last value makes queue empty again.
        auto v1 = a2.pop().unwrap();
        REQUIRE(a2.is_empty());
        NEL_UNUSED(v1);
    }

    // const queue cannot be push to.. copile fail.. how to test..unwrap().
}

TEST_CASE("heapless::Queue::len", "[heapless][queue]")
{
    {
        // empty queue must have len 0
        auto a1 = nel::heapless::Queue<int, 3>::empty();
        REQUIRE(a1.len() == 0);

        auto const c1 = nel::heapless::Queue<int, 3>::empty();
        REQUIRE(c1.len() == 0);
    }

    {
        // queue filled to length >0 must have len of length
        auto a3 = nel::heapless::Queue<int, 3>::empty();
        a3.push(1).unwrap();
        REQUIRE(a3.len() == 1);

        a3.push(2).unwrap();
        REQUIRE(a3.len() == 2);

        a3.push(3).unwrap();
        REQUIRE(a3.len() == 3);

        // maxed out queue cannot grow in size.
        a3.push(4).unwrap();
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
        a1.push(2).unwrap();
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
        a1.pop().unwrap();
        auto p4 = a1.push(5);
        REQUIRE(p4.is_ok());
    }
    {
        // pushing udts
        auto a1 = nel::heapless::Queue<Stub, 3>::empty();

        // empty queue can be push to  (what if size == 0).unwrap()?
        Stub::reset();
        auto p0 = a1.push(Stub(1));
        REQUIRE(p0.is_ok());
        // must have  only 1 instance in play..
        REQUIRE(Stub::instances == 1);
        // it as move-ctord into queue
        REQUIRE(Stub::move_ctor == 1);
        REQUIRE(Stub::move_assn == 0);

        // maxing out queue must work.
        a1.push(2).unwrap();
        auto p1 = a1.push(Stub(3));
        REQUIRE(p1.is_ok());
        // must have 3 instance in play..
        REQUIRE(Stub::instances == 3);
        // it as move-ctord into queue
        REQUIRE(Stub::move_ctor == 3);
        REQUIRE(Stub::move_assn == 0);

#if 0
        // over-doing it must fail
        auto p3 = a1.push(Stub(4));
        REQUIRE(p3.is_err());
        // must have 3 instance in play..
        REQUIRE(Stub::instances == 3);
#else
        // over-doing it must not fail
        // it must drop/delete the oldest value
        auto p3 = a1.push(Stub(4));
        REQUIRE(p3.is_ok());
        // must have 4 instance in play..
        // 3 in th e queue and 1 in the result returned from push fail.
        REQUIRE(Stub::instances == 4);
#endif

        // removing items allows more pushing
        auto g4 = a1.pop().unwrap();
        auto p4 = a1.push(Stub(5));
        REQUIRE(p4.is_ok());
    }

    {
        // and pushing then popping preserves order
        auto a2 = nel::heapless::Queue<int, 3>::empty();
        a2.push(1).unwrap();
        a2.push(2).unwrap();
        a2.push(3).unwrap();

        REQUIRE(a2.pop().unwrap() == 1);
        REQUIRE(a2.pop().unwrap() == 2);
        REQUIRE(a2.pop().unwrap() == 3);
    }

    {
        // and pushing more than capacity then popping preserves order
        // and drops oldest.
        auto a2 = nel::heapless::Queue<Stub, 3>::empty();
        Stub::reset();
        a2.push(Stub(1)).unwrap();
        a2.push(Stub(2)).unwrap();
        a2.push(Stub(3)).unwrap();
        a2.push(Stub(4)).unwrap();

        REQUIRE(a2.pop().unwrap().val == 2);
        REQUIRE(a2.pop().unwrap().val == 3);
        REQUIRE(a2.pop().unwrap().val == 4);
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

TEST_CASE("heapless::Queue::iter()", "[heapless][queue]")
{
#if defined(RUST_LIKE)
    {
        auto a1 = nel::heapless::Queue<int, 3>::empty();
        auto it = a1.iter();
        REQUIRE(it.next().is_none());
    }
    {
        auto a1 = nel::heapless::Queue<Stub, 3>::empty();
        a1.push(Stub(1)).unwrap();
        a1.push(Stub(2)).unwrap();

        auto it = a1.iter();
        REQUIRE(it.next().unwrap().val == 1);
        REQUIRE(it.next().unwrap().val == 2);
        REQUIRE(it.next().is_none());
    }
    {
        // iter over split range (i.e. current rp is not at beginning
        // and num items in queue wraps.
        auto a1 = nel::heapless::Queue<Stub, 3>::empty();
        a1.push(Stub(1)).unwrap();
        a1.push(Stub(2)).unwrap();
        a1.pop().unwrap();
        a1.push(Stub(3)).unwrap();
        a1.push(Stub(4)).unwrap();
        auto it = a1.iter();
        REQUIRE(it.next().unwrap().val == 2);
        REQUIRE(it.next().unwrap().val == 3);
        REQUIRE(it.next().unwrap().val == 4);
        REQUIRE(it.next().is_none());
    }
#endif

#if defined(C_LIKE)
    {
        auto a1 = nel::heapless::Queue<int, 3>::empty();
        auto it = a1.iter();
        REQUIRE(!it);
    }
    {
        auto a1 = nel::heapless::Queue<Stub, 3>::empty();
        a1.push(Stub(1)).unwrap();
        a1.push(Stub(2)).unwrap();
        auto it = a1.iter();
        REQUIRE(it);
        REQUIRE((*it).val == 1);
        ++it;
        REQUIRE(it);
        REQUIRE((*it).val == 2);
        ++it;
        REQUIRE(!it);
    }
    {
        // iter over split range (i.e. current rp is not at beginning
        // and num items in queue wraps.
        auto a1 = nel::heapless::Queue<Stub, 3>::empty();
        a1.push(Stub(1)).unwrap();
        a1.push(Stub(2)).unwrap();
        a1.pop().unwrap();
        a1.push(Stub(3)).unwrap();
        a1.push(Stub(4)).unwrap();
        auto it = a1.iter();
        REQUIRE(it);
        REQUIRE((*it).val == 2);
        ++it;
        REQUIRE(it);
        REQUIRE((*it).val == 3);
        ++it;
        REQUIRE(it);
        REQUIRE((*it).val == 4);
        ++it;
        REQUIRE(!it);
    }
#endif
}

} // namespace queue
} // namespace heapless
} // namespace test
} // namespace nel
