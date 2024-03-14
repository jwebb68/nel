// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-

#include <nel/iterator.hh>

#include <nel/heapless/vector.hh>

#include <nel/log.hh>
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

void do1(nel::Slice<int32_t>::Iterator &&it)
{
    auto fmt = nel::Formatter(nel::log);
    it.for_each([&fmt](int32_t const &e) { fmt << e << ','; });
}

#if defined(RUST_LIKE)
void do2(nel::Slice<int32_t>::Iterator &&it)
{
    auto fmt = nel::Formatter(nel::log);
    it.for_each2([&fmt](int32_t const &e) { fmt << e << ','; });
}
#endif

void do3(nel::Slice<int32_t>::Iterator &&it)
{
    auto fmt = nel::Formatter(nel::log);
    it.template map<int32_t>([](int32_t const &e) -> int32_t { return e * 100; })
        .first_n(4)
        .for_each([&fmt](int32_t const &e) { fmt << e << ','; });
}

#if defined(RUST_LIKE)
void do4(nel::Slice<int32_t>::Iterator &&it)
{
    auto fmt = nel::Formatter(nel::log);
    it.template map<int32_t>([](int32_t const &e) -> int32_t { return e * 100; })
        .first_n(4)
        .for_each2([&fmt](int32_t const &e) { fmt << e << ','; });
}
#endif

#if 0
// wont compile on arm
void do5(nel::Slice<int32_t>::Iterator &&it)
{
    auto fmt = nel::Formatter(nel::log);
    // loopy,  simple to read, slow..
    int32_t v = it.template map<int32_t>([](int32_t const & e) -> int32_t { return e * 100; })
                .first_n(400)
    .fold(10, [&](int32_t &acc, int32_t e) { acc += e; });
    fmt << v;
}

#    if defined(RUST_LIKE)
void do6(nel::Slice<int32_t>::Iterator &&it)
{
    auto fmt = nel::Formatter(nel::log);
    // compact, unrolled,  optimised.
    auto v = it.template map<int32_t>([](int32_t const & e) -> int32_t { return e * 100; })
             .first_n(400)
    .fold2(10, [&](int32_t &acc, int32_t const & e) { acc += e; });
    fmt << v;
}
#    endif
#endif

void ex1()
{
    auto a1 = nel::heapless::Vector<int32_t, 1000>();
    float v = 23;
    a1.iter().for_each([&](int32_t &e) {
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
    auto fmt = nel::Formatter(nel::log);

    fmt << "ex1:b" << '\n';
    ex1();
    fmt << "ex1:e" << '\n';
    return 0;
}
