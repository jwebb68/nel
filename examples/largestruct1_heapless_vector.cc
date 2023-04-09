// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heapless/vector.hh>
#include <nel/panic.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

typedef nel::heapless::Vector<U8Buf<256>, 10> Vec1;

void ex1()
{
    Vec1 vec1 = Vec1::empty();

    nel::log << vec1 << '\n';
}

void ex2()
{
    Vec1 vec1 = Vec1::empty();

    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    vec1.push(nel::move(v)).is_ok();

    nel::log << vec1 << '\n';
}

void ex3()
{
    Vec1 vec1 = Vec1::empty();

    vec1.push((uint8_t)0x67).is_ok();

    nel::log << vec1 << '\n';
}

#if defined(RUST_LIKE)
void ex41()
{
    Vec1 vec1 = Vec1::empty();

    vec1.push((uint8_t)0x67).is_ok();
    vec1.push((uint8_t)0x68).is_ok();
    vec1.push((uint8_t)0x69).is_ok();

    vec1.pop();

    auto it = vec1.iter();
    while (true) {
        auto e = it.next();
        // checking for none and unwrapping/ from opt
        // causes bigger loops
        // unwrap if some with other, with ind to say if value came from opt or passed in.
        // i.e. if let Some(x) = e {..}
        // up = e.unwrap_or_if(sentinel)
        // if (up[0]) { x = up[1]; ... }
        // T x;
        // if (e.some(x)) { ... }
        if (e.is_none()) { break; }
        // calls is_some() but always is_some..
        // opt does optimise it out but is there a better way?
        nel::log << e.unwrap() << '\n';
    }
}
#endif

void ex42()
{
    Vec1 vec1 = Vec1::empty();

    vec1.push((uint8_t)0x67).is_ok();
    vec1.push((uint8_t)0x68).is_ok();
    vec1.push((uint8_t)0x69).is_ok();

    vec1.pop();

    auto it = vec1.iter();
    for (; !it.is_done(); it.inc()) {
        auto &v = it.deref();
        nel::log << v << '\n';
    }
}

#if defined(RUST_LIKE)
void ex51()
{
    Vec1 vec1 = Vec1::empty();

    for (uint8_t f = 1; f < 10; ++f) {
        vec1.push(f).is_ok();
    }

    auto it = vec1.iter().first_n(3);
    while (true) {
        auto e = it.next();
        // checking for none and unwrapping/ from opt
        // causes bigger loops
        // unwrap if some with other, with ind to say if value came from opt or passed in.
        // i.e. if let Some(x) = e {..}
        // up = e.unwrap_or_if(sentinel)
        // if (up[0]) { x = up[1]; ... }
        // T x;
        // if (e.some(x)) { ... }
        if (e.is_none()) { break; }
        // calls is_some() but always is_some..
        // opt does optimise it out but is there a better way?
        nel::log << e.unwrap() << '\n';
    }
}
#endif

void ex52()
{
    Vec1 vec1 = Vec1::empty();

    for (uint8_t f = 1; f < 10; ++f) {
        vec1.push(f).is_ok();
    }

    auto it = vec1.iter().first_n(3);
    for (; !it.is_done(); it.inc()) {
        auto &v = it.deref();
        nel::log << v << '\n';
    }
}

int main()
{
    nel::log << "ex1:b" << '\n';
    ex1();
    nel::log << "ex1:e" << '\n';
    nel::log << "ex2:b" << '\n';
    ex2();
    nel::log << "ex2:e" << '\n';
    nel::log << "ex3:b" << '\n';
    ex3();
    nel::log << "ex3:e" << '\n';
#if defined(RUST_LIKE)
    nel::log << "ex41:b" << '\n';
    ex41();
    nel::log << "ex41:e" << '\n';
#endif
    nel::log << "ex42:b" << '\n';
    ex42();
    nel::log << "ex42:e" << '\n';
#if defined(RUST_LIKE)
    nel::log << "ex51:b" << '\n';
    ex51();
    nel::log << "ex51:e" << '\n';
#endif
    nel::log << "ex52:b" << '\n';
    ex52();
    nel::log << "ex52:e" << '\n';
}
