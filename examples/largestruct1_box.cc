#include "log.hh"
#include "defs.hh"
#include "largestruct1.hh"

#include "box.hh"

typedef nel::Box<U8Buf<256>> Box1;

nel::Log &operator<<(nel::Log &outs, Box1 const &v) {
    NEL_UNUSED(v);
    outs << "Box1()";
    return outs;
}

void box1() {
    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    Box1 b(std::move(v));

    nel::log << b << "\n";
}

void box2() {
    Box1 b((uint8_t)0x67);

    nel::log << b << "\n";
}


int main() {
}
