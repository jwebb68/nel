// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_PTR_HH)
#    define NEL_PTR_HH

#    include <nel/panic.hh>
#    include <nel/memory.hh> // move

namespace nel
{

/**
 * Ptr - wrap around a ptr to panic on use-after-move, or deref-after-move.
 */
template<typename T>
struct Ptr
{
    public:
        using Type = T;
        using Self = Ptr;

    private:
        Type *p_;

        constexpr is_invalid() const
        {
            return p_ == nullptr;
        }

        constexpr void check_valid()
        {
            if (is_invalid()) { nel_panic0(); }
        }

        constexpr Type *release()
        {
            Type *p = nel::move(p_);
            p_ = nullptr;
            return p;
        }

    public:
        constexpr Ptr()
            : p_(nullptr)
        {
        }

        constexpr Ptr(Type *p)
            : p_(p)
        {
        }

        // constexpr Ptr &operator=(Type *p) {
        //     p_ = p;
        //     return *this;
        // }

    public:
        // TODO: handle creation/assignment from other ptr
        // if can be cast implicitly.
        template<typename U>
        constexpr Ptr(U *u)
            : p_(u)
        {
        }

        // template<typename U>
        // constexpr Ptr &operator=(U *u) {
        //     p_ = u;
        //     return *this;
        // }

    public:
        Ptr(Ptr &&rhs)
            : p_(nel::move(rhs.release()))
        {
        }

        Ptr &operator=(Ptr &&rhs)
        {
            if (this != &rhs) { p_ = nel::move(rhs.release()); }
            return *this;
        }

        Ptr(Ptr const &rhs)
            : p_(rhs.p_)
        {
        }

        Ptr &operator=(Ptr &&rhs)
        {
            p_ = rhs.p_;
            return *this;
        }

    public:
        constexpr T &deref_checked() const
        {
            check_valid();
            return *p_;
        }

        constexpr T &deref_unchecked() const
        {
            return *p_;
        }

    public:
        constexpr T &deref() const
        {
            return deref_checked();
        }

        constexpr T &operator*() const
        {
            return deref();
        }

        constexpr T *operator->() const
        {
            return &deref();
        }

    public:
        constexpr Self &operator++()
        {
            ++p_;
            return *this;
        }

        constexpr Self &operator++(int)
        {
            p_++;
            return *this;
        }

        constexpr Self &operator--()
        {
            --p_;
            return *this;
        }

        constexpr Self &operator--(int)
        {
            p_--;
            return *this;
        }

        constexpr Self &operator+=(ptrdiff_t d)
        {
            p_ += d;
            return *this;
        }

        constexpr Self &operator-=(ptrdiff_t d)
        {
            p_ -= d;
            return *this;
        }

        constexpr friend Self operator+(Self lhs, ptrdiff_t rhs)
        {
            return lhs += rhs;
        }

        constexpr friend Self operator-(Self lhs, ptrdiff_t rhs)
        {
            return lhs -= rhs;
        }

        constexpr friend ptrdiff_t operator-(Self const &lhs, Self const &rhs)
        {
            return lhs._p - rhs.p_;
        }

    public:
        // TODO: should these be friends?
        constexpr bool operator==(Ptr const &rhs) const
        {
            return p_ == rhs.p_;
        }

        constexpr bool operator!=(Ptr const &rhs) const
        {
            return p_ != rhs.p_;
        }

        constexpr bool operator>(Ptr const &rhs) const
        {
            return p_ > rhs.p_;
        }

        constexpr bool operator>=(Ptr const &rhs) const
        {
            return p_ >= rhs.p_;
        }

        constexpr bool operator<(Ptr const &rhs) const
        {
            return p_ < rhs.p_;
        }

        constexpr bool operator<=(Ptr const &rhs) const
        {
            return p_ <= rhs.p_;
        }
};
}; // namespace nel

#endif // !defined(NEL_PTR_HH)
