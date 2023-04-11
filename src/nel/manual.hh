// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_MANUAL_HH)
#    define NEL_MANUAL_HH

namespace nel
{

template<typename T>
struct Manual;

};

#    include <nel/memory.hh> // move
#    include <inttypes.h>

namespace nel
{

/**
 * a manual type, disables all automatic handling usually done by the compiler
 * i.e. you, the developer, must now manually do them.
 * So construct/destroy etc operations the compiler would do.
 */
template<typename T>
struct Manual
{
    public:
        typedef T Type;

    private:
        // Use union so inspecting in a debugger is easier..
        // Don't use it for type punning.
        union {
                uint8_t buf_[sizeof(Type)];
                Type elem_;
        };

    public:
        // default must not delete..
        // doesn't know if value contained or not.
        constexpr ~Manual() {};

        // create unwrapping container
        constexpr Manual() {};

        // move value into container
        constexpr Manual(Type &&v)
        {
            new (ptr()) Type(move(v));
        }

        constexpr Manual &operator=(Type &&v)
        {
            // cannot use assign as that implies there is a value on
            // the lhs, which there may not be.
            // so, treat as new.
            // but don't pun to ctor as that would mess up cotr/assgn tracking.
            ref() = move(v);
            return *this;
        }

        template<typename... Args>
        constexpr Manual(Args &&...args)
        {
            new (ptr()) Type(forward<Args>(args)...);
        }

    public:
        // can move, but it's manual, so not automatic.
        constexpr Manual(Manual &&) {}

        constexpr Manual &operator=(Manual &&)
        {
            return *this;
        }

    public:
        // Can copy, but manually..
        // For the moment, disable copy.
        constexpr Manual(Manual const &) = delete;
        constexpr Manual &operator=(Manual const &) = delete;

    public:
        // only if you know there's a value there..
        constexpr Type *ptr()
        {
            // return &elem_;
            return static_cast<Type *>(static_cast<void *>(buf_));
        }

        constexpr Type const *ptr() const
        {
            // return &elem_;
            return static_cast<Type const *>(static_cast<void const *>(buf_));
        }

        constexpr Type *operator->()
        {
            return ptr();
        }

        constexpr Type const *operator->() const
        {
            return ptr();
        }

        constexpr Type &ref()
        {
            return *ptr();
        }

        constexpr Type const &ref() const
        {
            return *ptr();
        }

        constexpr Type &operator*()
        {
            return ref();
        }

        constexpr Type const &operator*() const
        {
            return ref();
        }

    public:
        constexpr void destroy()
        {
            ref().~Type();
        }

        // constexpr void move_in(Type &&v)
        // {
        //     ref() = move(v);
        // }
        // constexpr Type move_out(Type &&v)
        // {
        //     return move(ref());
        // }
};

}; // namespace nel

#endif //! defined(NEL_MANUAL_HH)
