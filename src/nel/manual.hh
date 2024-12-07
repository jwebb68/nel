// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_MANUAL_HH)
#    define NEL_MANUAL_HH

namespace nel
{

template<typename T>
struct Manual;

}

#    include <nel/memory.hh> // move
#    include <nel/new.hh> // new

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
        using Type = T;
        using Self = Manual;

    private:
        // Use union so inspecting in a debugger is easier..
        // Don't use it for type punning.
        // Also, union handles alignment.
        union U {
                uint8_t buf_[sizeof(Type)];
                Type elem_;

                constexpr ~U() {};
                constexpr U() {};
        } u_;

    public:
        // default must not delete..
        // doesn't know if value contained or not.
        constexpr ~Manual() = default;

        // create empty container
        // union should prevent ctor call of T.
        constexpr Manual() = default;

    public:
        // construct value in-place given an existing container.
        template<typename... Args>
        constexpr Self &construct(Args &&...args)
        {
            new (ptr()) Type(forward<Args>(args)...);
            return *this;
        }

        // destruct contained value
        constexpr Self &destruct()
        {
            deref().~Type();
            return *this;
        }

    public:
        // move value into container
        constexpr Manual(Type &&v)
        {
            // use of this means dev is asserting that there is not a value already
            // contained.
            new (ptr()) Type(move(v));
        }

        constexpr Self &operator=(Type &&v)
        {
            // use of this means dev is asserting that there is a value already
            // contained.
            return move_from(nel::move(v));
        }

    public:
        // create value inplace in new container.
        template<typename... Args>
        constexpr Manual(Args &&...args)
        {
            construct(forward<Args>(args)...);
        }

    public:
        // can move, but it's manual, so not automatic.
        constexpr Manual(Manual &&) {}

        constexpr Self &operator=(Manual &&)
        {
            return *this;
        }

        constexpr Self &move_from(Manual &&rhs)
        {
            deref() = nel::move(rhs.ref());
            return *this;
        }

        constexpr Self &move_from(Type &&v)
        {
            deref() = nel::move(v);
            return *this;
        }

    public:
        // Can copy, but manually..
        // For the moment, disable copy.
        constexpr Manual(Manual const &) = delete;
        constexpr Manual &operator=(Manual const &) = delete;

        constexpr Self &copy_from(Manual const &rhs)
        {
            deref() = rhs.ref();
            return *this;
        }

        // private:
    public:
        // only if you know there's a value there..
        // TODO: must have no bare pointers.. safety feature.
        constexpr Type *ptr()
        {
            return &u_.elem_;
        }

        constexpr Type const *ptr() const
        {
            return &u_.elem_;
        }

    public:
        // only if you know there's a value there..
        // yes, bare pointer, but usually used as a member accessor.
        constexpr Type *operator->()
        {
            return ptr();
        }

        constexpr Type const *operator->() const
        {
            return ptr();
        }

        // only if you know there's a value there..
        constexpr Type &deref()
        {
            return *ptr();
        }

        constexpr Type const &deref() const
        {
            return *ptr();
        }

        // only if you know there's a value there..
        constexpr Type &operator*()
        {
            return deref();
        }

        constexpr Type const &operator*() const
        {
            return deref();
        }
};

} // namespace nel

#endif //! defined(NEL_MANUAL_HH)
