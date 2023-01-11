#include "largestruct1.hh"

#include <nel/heaped/vector.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

#include <utility> // std::move

typedef nel::heaped::Vector<U8Buf<256>> Vec1;

void ex1() {
    Vec1 vec1 = Vec1::with_capacity(5);

    nel::log << vec1 << '\n';
}

void ex2() {
    Vec1 vec1 = Vec1::with_capacity(5);

    U8Buf<256> v = U8Buf<256>((uint8_t)0x66);
    vec1.push_back(std::move(v));

    nel::log << vec1 << '\n';
}

void ex3() {
    Vec1 vec1 = Vec1::with_capacity(5);

    vec1.push_back((uint8_t)0x67);

    nel::log << vec1 << '\n';
}

void ex4() {
    auto r = Vec1::try_from({
        ((uint8_t)0x67),
        ((uint8_t)0x68),
        ((uint8_t)0x69),
    });
    Vec1 vec1 = r.unwrap();

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
        if (e.is_none()) {
            break;
        }
        // calls is_some() but always is_some..
        // opt does optimise it out but is there a better way?
        nel::log << e.unwrap() << '\n';
    }
}


int main() {
    nel::log << "ex1:b" << '\n';
    ex1();
    nel::log << "ex1:e" << '\n';
    nel::log << "ex2:b" << '\n';
    ex2();
    nel::log << "ex2:e" << '\n';
    nel::log << "ex3:b" << '\n';
    ex3();
    nel::log << "ex3:e" << '\n';
    nel::log << "ex4:b" << '\n';
    ex4();
    nel::log << "ex4:e" << '\n';
}
