// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_HEAPED_BOX_HH)
#    define NEL_HEAPED_BOX_HH

namespace nel
{
namespace heaped
{

template<typename T>
struct Box;

} // namespace heaped
} // namespace nel

#    include <nel/result.hh>
#    include <nel/element.hh>
#    include <nel/panic.hh>
#    include <nel/defs.hh>
#    include <nel/memory.hh> // move,forward

namespace nel
{
namespace heaped
{

/**
 * A T on the heap, owned (a bit like std::unique_ptr)
 */
template<typename T>
struct Box
{
    public:
        // heh, blatant rust-ism
        typedef Box Self;
        typedef T Type;

    private:
        typedef Element<Type> ElementT;
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

        constexpr Box(Type &&v)
            : value_(new ElementT(forward<Type>(v)))
        {
        }

        // works for moving-into as well.
        template<typename... Args>
        constexpr Box(Args &&...args)
            : value_(new ElementT(forward<Args>(args)...))
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
        constexpr static Result<Box, Type> try_from(Type &&val)
        {
            // for new: on fail, val not moved
            ElementT *const p = new ElementT(move(val));
            if (p == nullptr) { return Result<Box, Type>::Err(move(val)); }
            return Result<Box, Type>::Ok(move(Box(p)));
        }

    public:
        /**
         * Return a mutable reference to the boxed value.
         *
         * @returns reference to the boxed value.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr Type &operator*(void)
        {
            return deref();
        }

        constexpr Type &deref(void)
        {
            nel_panic_if_not(has_value(), "not a value");
            return *(*value_);
        }

        /**
         * Return a const reference to the boxed value.
         *
         * @returns reference to the boxed value.
         * @warning Panics if no value to return (e.g. use after move).
         */
        constexpr Type const &operator*(void) const
        {
            return deref();
        }

        constexpr Type const &deref(void) const
        {
            nel_panic_if_not(has_value(), "not a value");
            return *(*value_);
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
        constexpr Type unwrap(void)
        {
            nel_panic_if_not(has_value(), "not a value");
            Type t = move(*(*value_));
            delete value_;
            value_ = nullptr;
            return t;
        }

        // No comparators by design.
        // (i.e. Keep it simple)
};

} // namespace heaped
} // namespace nel

#endif // !defined(NEL_HEAPED_BOX_HH)
