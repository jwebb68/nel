// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heaped/rc.hh>
#include <nel/log.hh>
#include <nel/memory.hh> // move()
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

typedef nel::heaped::RC<U8Buf<256>> RC1;

void rc1()
{
    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    RC1 rc1(nel::move(v));

    auto fmt = nel::Formatter(nel::log);
    rc1.dbgfmt(fmt) << '\n';
}

void rc2()
{
    RC1 rc1((uint8_t)0x67);

    auto fmt = nel::Formatter(nel::log);
    rc1.dbgfmt(fmt) << '\n';
}

int main() {}
