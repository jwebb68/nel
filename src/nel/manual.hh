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
    private:
        // Use union so inspecting in a debugger is easier..
        // Don't use it for type punning.
        union {
                uint8_t buf_[sizeof(T)];
                T elem_;
        };

    public:
        // default must not delete..
        // doesn't know if value contained or not.
        constexpr ~Manual() {};

        // create unwrapping container
        constexpr Manual() {};

        // move value into container
        constexpr Manual(T &&v)
        {
            new (ptr()) T(move(v));
        }

        constexpr Manual &operator=(T &&v)
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
            new (ptr()) T(forward<Args>(args)...);
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
        constexpr T *ptr()
        {
            // return &elem_;
            return static_cast<T *>(static_cast<void *>(buf_));
        }

        constexpr T const *ptr() const
        {
            // return &elem_;
            return static_cast<T const *>(static_cast<void const *>(buf_));
        }

        constexpr T *operator->()
        {
            return ptr();
        }

        constexpr T const *operator->() const
        {
            return ptr();
        }

        constexpr T &ref()
        {
            return *ptr();
        }

        constexpr T const &ref() const
        {
            return *ptr();
        }

        constexpr T &operator*()
        {
            return ref();
        }

        constexpr T const &operator*() const
        {
            return ref();
        }

    public:
        constexpr void destroy()
        {
            ref().~T();
        }

        // constexpr void move_in(T &&v)
        // {
        //     ref() = move(v);
        // }
        // constexpr T move_out(T &&v)
        // {
        //     return move(ref());
        // }
};

}; // namespace nel

#endif //! defined(NEL_MANUAL_HH)
