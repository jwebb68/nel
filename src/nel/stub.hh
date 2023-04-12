// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_STUB_HH)
#    define NEL_STUB_HH

namespace nel
{
namespace test
{

struct Stub;

};
}; // namespace nel

#    include <nel/memory.hh> // move

namespace nel
{
namespace test
{

struct Stub
{
        static int instances;
        static int move_ctor;
        static int move_assn;
        static int copy_ctor;
        static int copy_assn;

        static void reset()
        {
            instances = 0;
            move_ctor = 0;
            move_assn = 0;
            copy_ctor = 0;
            copy_assn = 0;
        }

        bool valid;
        int val;

        ~Stub()
        {
            instances -= 1;
        }

        Stub(int v)
            : valid(true)
            , val(v)
        {
            instances += 1;
        }

        Stub(Stub &&o)
            : valid(move(o.valid))
            , val(move(o.val))
        {
            // move-ctor: moves value (new value not created)
            // move-ctor: src location is invalidated but not deleted. (that comes later)
            // move-assn: dst location is validated, initialised.
            // remember, the dtor of o will be called when it goes out of scope.
            o.valid = false;
            instances += 1;
            move_ctor += 1;
        }

        Stub &operator=(Stub &&o)
        {
            // move-assn: moves value (new value not created)
            // move-assn: src location is invalidated but not deleted. (that comes later)
            // move-assn: dst location is validated but not deleted. (reused)
            // remember, the dtor of o will be called when it goes out of scope.
            val = nel::move(o.val);
            valid = nel::move(o.valid);
            o.valid = false;
            move_assn += 1;
            return *this;
        }

        Stub(Stub const &o)
            : valid(o.valid)
            , val(o.val)
        {
            // copy-ctor: copies value (new value is created)
            // copy-ctor: src location is left alone, not deleted.
            // copy-ctor: dst location is initialised.
            instances += 1;
            copy_ctor += 1;
        }

        Stub &operator=(Stub const &o)
        {
            // copy-assn: copies value (new value is created)
            // copy-ctor: src location is left alone, not deleted.
            // copy-ctor: dst location is re-initialised (reused)
            valid = o.valid;
            val = o.val;
            copy_assn += 1;
            return *this;
        }

        Stub() = delete;

        constexpr bool operator==(Stub const &o) const
        {
            return valid && val == o.val;
        }

        constexpr bool operator!=(Stub const &o) const
        {
            return valid && val != o.val;
        }
};

}; // namespace test
}; // namespace nel

#endif //! defined(NEL_STUB_HH)
