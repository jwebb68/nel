// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_TYPES_HH)
#    define NEL_TYPES_HH

namespace nel
{
    // because specialising templates for void is a sodding pain.
    // so use a 'null' type.
    struct Void {
        public:
            friend constexpr bool operator==(Void const &/*lhs*/, Void const &/*rhs*/)
            {
                return true;
            }
            friend constexpr bool operator!=(Void const &/*lhs*/, Void const &/*rhs*/)
            {
                return false;
            }
    };
}

#endif//!defined(NEL_TYPES_HH)
