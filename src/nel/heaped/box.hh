// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#ifndef NEL_HEAPED_BOX_HH
#define NEL_HEAPED_BOX_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct Box;

} // namespace heaped
} // namespace nel

#include <nel/result.hh>
#include <nel/element.hh>
#include <nel/panic.hh>
#include <nel/defs.hh>

namespace nel
{
namespace heaped
{

/**
 * A T on the heap, owned (a bit like std::unique_ptr)
 */
template<typename T>
struct Box {
    public:
        // heh, blatant rust-ism
        typedef Box Self;

    private:
        typedef Element<T> ElementT;
        ElementT *value_;

    private:
        constexpr Box(ElementT *const p)
            : value_(p)
        {
        }

    public:
        ~Box(void)
        {
            if (value_ != nullptr) { delete value_; }
        }

        // No default, must create a T.
        // constexpr Box(void)  = delete;
        constexpr Box(void)
            : value_(nullptr)
        {
        }

        // No copying..
        constexpr Box(Box const &o) = delete;
        constexpr Box &operator=(Box const &o) = delete;

        // Can move though.
        constexpr Box(Box &&o)
        {
            value_ = o.value_;
            o.value_ = nullptr;
        }

        constexpr Box &operator=(Box &&o)
        {
            if (this != &o) {
                value_ = o.value_;
                o.value_ = nullptr;
            }
            return *this;
        }

        constexpr Box(T &&v)
            : value_(new Element<T>(forward<T>(v)))
        {
        }

        // works for moving-into as well.
        template<typename... Args>
        constexpr Box(Args &&...args)
            : value_(new Element<T>(forward<Args>(args)...))
        {
        }

    public:
        /**
         * Create a boxed T by moving existing into the box.
         *
         * @param val The value to move into the box
         * @returns on success, Optional::Some
         * @returns of fail, Optional::None
         */
        constexpr static Result<Box, T> try_from(T &&val)
        {
            // for new: on fail, val not moved
            ElementT *const p = new ElementT(move(val));
            if (p == nullptr) { return Result<Box, T>::Err(val); }
            return Result<Box, T>::Ok(Box(p));
        }

    public:
        /**
         * Return a mutable reference to the boxed value.
         *
         * @returns reference to the boxed value.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T &operator*(void)
        {
            return deref();
        }

        constexpr T &deref(void)
        {
            nel_panic_if_not(has_value(), "not a value");
            return value_->get();
        }

        /**
         * Return a const reference to the boxed value.
         *
         * @returns reference to the boxed value.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T const &operator*(void) const
        {
            return deref();
        }

        constexpr T const &deref(void) const
        {
            nel_panic_if_not(has_value(), "not a value");
            return value_->get();
        }

        /**
         * Determines if this box has a value.
         *
         * @returns true if there is a value, false otherwise.
         */
        constexpr bool has_value(void) const
        {
            return value_ != nullptr;
        }

        constexpr operator bool(void) const
        {
            return has_value();
        }

        /**
         * Removes and returns the value contained in the box.
         *
         * The box is invalidated by this action.
         *
         * @returns the value in the box.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr T unwrap(void)
        {
            nel_panic_if_not(has_value(), "not a value");
            T t = value_->unwrap();
            delete value_;
            value_ = nullptr;
            return t;
        }

        // No comparators by design.
        // (i.e. Keep it simple)
};

} // namespace heaped
} // namespace nel

#endif // NEL_HEAPED_BOX_HH
