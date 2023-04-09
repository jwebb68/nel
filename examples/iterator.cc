// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-

#include <nel/iterator.hh>

#include <nel/heapless/vector.hh>

#include <nel/log.hh>
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

void do1(nel::Slice<int>::Iterator &&it)
{
    it.for_each([&](int const &e) { nel::log << e << ','; });
}

#if defined(RUST_LIKE)
void do2(nel::Slice<int>::Iterator &&it)
{
    it.for_each2([&](int const &e) { nel::log << e << ','; });
}
#endif

void do3(nel::Slice<int>::Iterator &&it)
{
    it.template map<int>([](int const &e) -> int { return e * 100; })
        .first_n(4)
        .for_each([&](int const &e) { nel::log << e << ','; });
}

#if defined(RUST_LIKE)
void do4(nel::Slice<int>::Iterator &&it)
{
    it.template map<int>([](int const &e) -> int { return e * 100; })
        .first_n(4)
        .for_each2([&](int const &e) { nel::log << e << ','; });
}
#endif

void do5(nel::Slice<int>::Iterator &&it)
{
    // loopy,  simpe to read, slow..
    int v = it.template map<int>([](int const &e) -> int { return e * 100; })
                .first_n(400)
                .fold(10, [&](int &acc, int &e) { acc += e; });
    nel::log << v;
}

#if defined(RUST_LIKE)
void do6(nel::Slice<int>::Iterator &&it)
{
    // compact, unrolled,  optimised.
    int v = it.template map<int>([](int const &e) -> int { return e * 100; })
                .first_n(400)
                .fold2(10, [&](int &acc, int const &e) { acc += e; });
    nel::log << v;
}
#endif

void ex1()
{
    auto a1 = nel::heapless::Vector<int, 1000>();
    float v = 23;
    a1.iter().for_each([&](int &e) {
        e = v;
        v += 34;
    });
    do1(a1.iter());
#if defined(RUST_LIKE)
    do2(a1.iter());
#endif

    do3(a1.iter());
#if defined(RUST_LIKE)
    do4(a1.iter());
#endif
    do5(a1.iter());
#if defined(RUST_LIKE)
    do6(a1.iter());
#endif
}

int main()
{
    nel::log << "ex1:b" << '\n';
    ex1();
    nel::log << "ex1:e" << '\n';
}
