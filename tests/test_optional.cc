#include "optional.hh"


nel::OptionalT<int> foo_some()
{
    return nel::Some(1);
}
nel::OptionalT<int> foo_none()
{
    return nel::None();
}

extern "C"
bool test_optional_none()
{
    auto res = foo_none();
    return res.is_none();
}

extern "C"
bool test_optional_some()
{
    auto res = foo_some();
    return res.is_some();
}

#define UNUSED(x)  ((void)(x))

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    test_optional_none();
    test_optional_some();
    return 0;
}
