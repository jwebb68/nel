// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#include "stub.hh"

namespace nel
{
namespace test
{

int Stub::instances = 0;
int Stub::dtor = 0;
int Stub::move_ctor = 0;
int Stub::move_assn = 0;
int Stub::copy_ctor = 0;
int Stub::copy_assn = 0;

} // namespace test
} // namespace nel
