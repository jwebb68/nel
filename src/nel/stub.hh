// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_STUB_HH)
#    define NEL_STUB_HH

namespace nel
{
namespace test
{

struct Stub;

}
} // namespace nel

#    include <nel/memory.hh> // move

namespace nel
{
namespace test
{

struct Stub
{
        static int instances;
        static int ctor;
        static int dtor;
        static int move_ctor;
        static int move_assn;
        static int copy_ctor;
        static int copy_assn;
        static int foo_calls;

        static void reset()
        {
            instances = 0;
            ctor = 0;
            dtor = 0;
            move_ctor = 0;
            move_assn = 0;
            copy_ctor = 0;
            copy_assn = 0;
            foo_calls = 0;
        }

        bool valid;
        int val;

        ~Stub()
        {
            dtor += 1;
            if (valid) { instances -= 1; }
        }

        Stub(int v)
            : valid(true)
            , val(v)
        {
            ctor += 1;
            instances += 1;
        }

        Stub(Stub &&rhs)
            : valid(move(rhs.valid))
            , val(move(rhs.val))
        {
            // move-ctor: moves value (new value not created)
            // move-ctor: src location is invalidated but not deleted. (that comes later)
            // move-assn: dst location is validated, initialised.
            // remember, the dtor of o will be called when it goes out of scope.
            rhs.valid = false;
            move_ctor += 1;
            // no new value instances.
        }

        Stub &operator=(Stub &&rhs)
        {
            // move-assn: moves value (new value not created)
            // move-assn: src location is invalidated but not deleted. (that comes later)
            // move-assn: dst location is validated but not deleted. (reused)
            // remember, the dtor of o will be called when it goes out of scope.
            if (&rhs != this) {
                if (valid && rhs.valid) {
                    val = nel::move(rhs.val);
                    // no new value created.
                } else if (!valid && rhs.valid) {
                    // new (&val) int(nel::move(rhs.val));
                    val = nel::move(rhs.val);
                } else if (!valid && !rhs.valid) {
                    // do nothing
                } else if (valid && !rhs.valid) {
                    // destruct it.
                }
                valid = nel::move(rhs.valid);
                rhs.valid = false;
                move_assn += 1;
            }
            return *this;
        }

        Stub(Stub const &rhs)
            : valid(rhs.valid)
            , val(rhs.val)
        {
            // copy-ctor: copies value (new value is created)
            // copy-ctor: src location is left alone, not deleted.
            // copy-ctor: dst location is initialised.
            instances += 1;
            copy_ctor += 1;
        }

        Stub &operator=(Stub const &rhs)
        {
            // copy-assn: copies value (new value is created)
            // copy-ctor: src location is left alone, not deleted.
            // copy-ctor: dst location is re-initialised (reused)
            if (&rhs != this) {
                if (valid && rhs.valid) {
                    val = rhs.val;
                    // no new value created.
                } else if (!valid && rhs.valid) {
                    // new (&val) int(rhs.val);
                    val = rhs.val;
                    instances += 1;
                } else if (!valid && !rhs.valid) {
                    // do nothing
                } else if (valid && !rhs.valid) {
                    // destruct it.
                }
                valid = rhs.valid;
                copy_assn += 1;
            }
            return *this;
        }

        Stub() = delete;

        constexpr friend bool operator==(Stub const &lhs, Stub const &rhs)
        {
            if (!lhs.valid) { return false; }
            if (!rhs.valid) { return false; }
            return lhs.val == rhs.val;
        }

        constexpr friend bool operator!=(Stub const &lhs, Stub const &rhs)
        {
            if (!lhs.valid) { return false; }
            if (!rhs.valid) { return false; }
            return lhs.val != rhs.val;
        }

    public:
        void foo() const
        {
            foo_calls += 1;
        }
};

} // namespace test
} // namespace nel

#endif //! defined(NEL_STUB_HH)
