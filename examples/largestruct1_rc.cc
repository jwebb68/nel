// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/heaped/rc.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

#include <utility> // std::move

typedef nel::heaped::RC<U8Buf<256>> RC1;

nel::Log &operator<<(nel::Log &outs, RC1 const &v)
{
    NEL_UNUSED(v);
    outs << "RC1()";
    return outs;
}

void rc1()
{
    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    RC1 rc1(std::move(v));

    nel::log << rc1 << '\n';
}

void rc2()
{
    RC1 rc1((uint8_t)0x67);

    nel::log << rc1 << '\n';
}

int main() {}
