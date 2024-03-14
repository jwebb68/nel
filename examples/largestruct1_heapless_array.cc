// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heapless/array.hh>
#include <nel/panic.hh>
#include <nel/log.hh>
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

typedef nel::heapless::Array<U8Buf<256>, 8> Array1;

void ex1()
{
    // what should this give?
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));

    auto fmt = nel::Formatter(nel::log);
    arr1.dbgfmt(fmt) << '\n';
}

#if 0
void ex2()
{
    Array1 arr1 {
        U8Buf<256>(0x26),
        U8Buf<256>(0x27),
        U8Buf<256>(0x28),
        U8Buf<256>(0x29),
        U8Buf<256>(0x2a),
        U8Buf<256>(0x2b),
        U8Buf<256>(0x2c),
        U8Buf<256>(0x2d),
    };

    auto fmt = nel::Formatter(nel::log);
    arr1.dbgfmt(fmt) << '\n';
}
#endif

#if 0
void ex3()
{
    Array1 arr1 {
        {(uint8_t)0x36},
        {(uint8_t)0x37},
        {(uint8_t)0x38},
        {(uint8_t)0x39},
        {(uint8_t)0x3a},
        {(uint8_t)0x3b},
        {(uint8_t)0x3c},
        {(uint8_t)0x3d},
    };

    auto fmt = nel::Formatter(nel::log);
    arr1.dbgfmt(fmt) << '\n';
}
#endif

void ex3()
{
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));

    arr1.try_get(0).unwrap() = U8Buf<256>(0x11);
    arr1.try_get(5).unwrap() = U8Buf<256>(0x22);
    arr1.try_get(6).unwrap() = U8Buf<256>(0x33);
}

#if defined(RUST_LIKE)
void ex41()
{
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));

    auto it = arr1.iter();
    auto fmt = nel::Formatter(nel::log);
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
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));

    auto fmt = nel::Formatter(nel::log);
    arr1.iter().for_each([&fmt](U8Buf<256> &v) -> void { v.dbgfmt(fmt) << '\n'; });
}

#if defined(C_LIKE)
void ex51()
{
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));
    auto fmt = nel::Formatter(nel::log);

    auto it = arr1.iter();
    for (; !it.is_done(); it.inc()) {
        auto &v = it.deref();
        v.dbgfmt(fmt) << '\n';
    }
}

void ex52()
{
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));

    auto fmt = nel::Formatter(nel::log);
#    if defined(RUST_LIKE)
    arr1.iter().for_each2([&fmt](U8Buf<256> &v) -> void { v.dbgfmt(fmt) << '\n'; });
#    elif defined(C_LIKE)
    arr1.iter().for_each([&fmt](U8Buf<256> &v) -> void { v.dbgfmt(fmt) << '\n'; });
#    endif
}
#endif

#if defined(RUST_LIKE)
void ex61()
{
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));
    auto fmt = nel::Formatter(nel::log);

    auto it = arr1.iter().first_n(2);
    nel::Index i = 0;
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
        fmt << '[' << i << ']' << ':';
        e.unwrap().dbgfmt(fmt) << '\n';
        ++i;
    }
}
#endif

void ex62()
{
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));
    auto fmt = nel::Formatter(nel::log);

    auto it = arr1.iter().first_n(2);
    nel::Index i = 0;
    it.for_each([&](U8Buf<256> &e) -> void {
        fmt << '[' << i << ']' << ':';
        e.dbgfmt(fmt) << '\n';
        ++i;
    });
}

#if defined(C_LIKE)
void ex71()
{
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));
    auto fmt = nel::Formatter(nel::log);

    auto it = arr1.iter().first_n(2);
    nel::Index i = 0;
    for (; !it.is_done(); it.inc()) {
        // user can miss out the &, so forcing a copy/move of items being enumerated.
        auto &e = it.deref();
        fmt << '[' << i << ']' << ':';
        e.dbgfmt(fmt) << '\n';
        ++i;
    }
}

void ex72()
{
    Array1 arr1 = Array1::filled_with(U8Buf<256>(0xff));
    auto fmt = nel::Formatter(nel::log);

    auto it = arr1.iter().first_n(2);
    nel::Index i = 0;
#    if defined(RUST_LIKE)
    it.for_each2([&](U8Buf<256> &e) -> void {
        fmt << '[' << i << ']' << ':';
        fmt.dbgfmt(e) << '\n';
        ++i;
    });
#    elif defined(C_LIKE)
    it.for_each([&](U8Buf<256> &e) -> void {
        fmt << '[' << i << ']' << ':';
        e.dbgfmt(fmt) << '\n';
        ++i;
    });
#    endif
}
#endif

int main()
{
    auto fmt = nel::Formatter(nel::log);

    fmt << "ex1:b" << '\n';
    ex1();
    fmt << "ex1:e" << '\n';
    // fmt << "ex2:b" << '\n';
    // ex2();
    // fmt << "ex2:e" << '\n';
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
#if defined(C_LIKE)
    fmt << "ex51:b" << '\n';
    ex51();
    fmt << "ex51:e" << '\n';
    fmt << "ex52:b" << '\n';
    ex52();
    fmt << "ex52:e" << '\n';
#endif
#if defined(RUST_LIKE)
    fmt << "ex61:b" << '\n';
    ex61();
    fmt << "ex61:e" << '\n';
#endif
    fmt << "ex62:b" << '\n';
    ex62();
    fmt << "ex62:e" << '\n';

#if defined(C_LIKE)
    fmt << "ex71:b" << '\n';
    ex71();
    fmt << "ex71:e" << '\n';

    fmt << "ex72:b" << '\n';
    ex72();
    fmt << "ex72:e" << '\n';
#endif
}
