#ifndef NEL_NONE_HH
#define NEL_NONE_HH

namespace nel
{

class NoneT;


class NoneT
{
    public:
};
constexpr NoneT None() noexcept
{
    return NoneT();
}

}


#endif//NEL_NONE_HH
