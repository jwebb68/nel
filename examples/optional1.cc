#include "optional.hh"


nel::Optional<int> foo_some()
{
    return nel::Optional<int>::Some(1);
}
nel::Optional<int> foo_none()
{
    return nel::Optional<int>::None();
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
