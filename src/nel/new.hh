// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_NEW_HH)
#    define NEL_NEW_HH

#    include <nel/defs.hh>

#    if !defined(TEST)
// catch2 uses stdlib and so uses stdlib's new defn.
// Operator new must not be in a namespace
inline void *operator new(nel::USize, void *p)
{
    return p;
}
#    endif

#endif //! defined(NEL_NEW_HH)
