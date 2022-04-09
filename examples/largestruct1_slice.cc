#include "largestruct1.hh"

#include <nel/heaped/vector.hh>
#include <nel/log.hh>
#include <nel/defs.hh>

typedef nel::heaped::Vector<U8Buf<256>> Vec1;

void ex1(void) {
    Vec1 vec1;

    vec1.push_back((uint8_t)0x67);
    vec1.push_back((uint8_t)0x68);
    vec1.push_back((uint8_t)0x69);

    auto s1 = vec1.slice();
    nel::log << s1.len() << "\n";

}

int main()
{
}
