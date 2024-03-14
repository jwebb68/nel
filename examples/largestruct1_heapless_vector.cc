// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heapless/vector.hh>
#include <nel/panic.hh>
#include <nel/log.hh>
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

typedef nel::heapless::Vector<U8Buf<256>, 10> Vec1;

void ex1()
{
    Vec1 vec1 = Vec1::empty();

    auto fmt = nel::Formatter(nel::log);
    vec1.dbgfmt(fmt) << '\n';
}

void ex2()
{
    Vec1 vec1 = Vec1::empty();

    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    vec1.push(nel::move(v)).is_ok();

    auto fmt = nel::Formatter(nel::log);
    vec1.dbgfmt(fmt) << '\n';
}

void ex3()
{
    Vec1 vec1 = Vec1::empty();

    vec1.push((uint8_t)0x67).is_ok();

    auto fmt = nel::Formatter(nel::log);
    vec1.dbgfmt(fmt) << '\n';
}

#if defined(RUST_LIKE)
void ex41()
{
    Vec1 vec1 = Vec1::empty();
    auto fmt = nel::Formatter(nel::log);

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
        e.unwrap().dbgfmt(fmt) << '\n';
    }
}
#endif

void ex42()
{
    Vec1 vec1 = Vec1::empty();
    auto fmt = nel::Formatter(nel::log);

    vec1.push((uint8_t)0x67).is_ok();
    vec1.push((uint8_t)0x68).is_ok();
    vec1.push((uint8_t)0x69).is_ok();

    vec1.pop();

    auto it = vec1.iter();
    for (; !it.is_done(); it.inc()) {
        auto &v = it.deref();
        v.dbgfmt(fmt) << '\n';
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
        e.unwrap().dbgfmt(fmt) << '\n';
    }
}
#endif

void ex52()
{
    Vec1 vec1 = Vec1::empty();
    auto fmt = nel::Formatter(nel::log);

    for (uint8_t f = 1; f < 10; ++f) {
        vec1.push(f).is_ok();
    }

    auto it = vec1.iter().first_n(3);
    for (; !it.is_done(); it.inc()) {
        auto &v = it.deref();
        v.dbgfmt(fmt) << '\n';
    }
}

int main()
{
    auto fmt = nel::Formatter(nel::log);
    fmt << "ex1:b" << '\n';
    ex1();
    fmt << "ex1:e" << '\n';
    fmt << "ex2:b" << '\n';
    ex2();
    fmt << "ex2:e" << '\n';
    fmt << "ex3:b" << '\n';
    ex3();
    fmt << "ex3:e" << '\n';
#if defined(RUST_LIKE)
    fmt << "ex41:b" << '\n';
    ex41();
    fmt << "ex41:e" << '\n';
#endif
    fmt << "ex42:b" << '\n';
    ex42();
    fmt << "ex42:e" << '\n';
#if defined(RUST_LIKE)
    fmt << "ex51:b" << '\n';
    ex51();
    fmt << "ex51:e" << '\n';
#endif
    fmt << "ex52:b" << '\n';
    ex52();
    fmt << "ex52:e" << '\n';
}
