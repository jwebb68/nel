// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "largestruct1.hh"

#include <nel/manual.hh>

#include <nel/log.hh>
#include <nel/defs.hh>
#include "libnosys_stubs.cx"

using Stub = U8Buf<256>;

void ex1(void)
{
    auto m1 = nel::Manual<Stub>();

    m1.construct();

    nel::log << *m1 << '\n';

    m1.destruct();
}

struct RegBlock
{
    public:
        uint32_t reg1_;

    public:
        static RegBlock volatile &BLOCK0;
};

inline RegBlock volatile &RegBlock::BLOCK0 = *reinterpret_cast<RegBlock volatile *>(0x4000'0000);

struct Fake
{
        void foo()
        {
            RegBlock::BLOCK0.reg1_ = 23;
        }
};

nel::Manual<Fake> fak_;

extern "C" void isr_ctimer()
{
    // there must be 3 instrs.
    // 1 load reg with value
    // 2 load reg with address (precomputed, no alignment adjusts etc)
    // 3 store value to address.
    fak_->foo();
}

int main()
{
    nel::log << "ex1:b" << '\n';
    ex1();
    nel::log << "ex1:e" << '\n';
}
