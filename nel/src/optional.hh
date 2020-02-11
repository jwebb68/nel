#ifndef NEL_OPTIONAL_HH
#define NEL_OPTIONAL_HH

namespace nel
{

template<typename T>
class Optional;

}


#include <exception> //std::terminate

namespace nel
{

template<typename T>
class Optional
{
    public:
        class Some
        {
            private:
                T value_;

            public:
                explicit Some(const T &other) noexcept:
                    value_(other)
                {}

            public:
                // TODO: unwrap or deref operator for value_ access?
                const T &unwrap() const noexcept
                {
                    return this->value_;
                }

                T unwrap() noexcept
                {
                    return this->value_;
                }
        };

        class None
        {
            public:
                None() {}
        };

    private:
        enum
        {
            NONE = 0,
            SOME
        } tag_;

        union
        {
            Some some_;
            // None none_;
        };

    public:
        ~Optional() noexcept
        {
            switch (this->tag_)
            {
                case SOME:
                    this->some_.Some::~Some();
                    break;

                case NONE:
                    // this->none_.None::~None();
                    break;

                default:
                    std::terminate();
                    break;
            }
        }

        Optional(const Optional &other) noexcept:
            tag_(other.tag_)
        {
            switch (other.tag_)
            {
                case SOME:
                    this->some_ = other.some_;
                    break;

                case NONE:
                    // this->none_ = other.none_;
                    break;

                default:
                    std::terminate();
                    break;

            }
        }

        Optional(void) noexcept:
            tag_(NONE)
            //, none_()
        {}

        Optional(const None &) noexcept:
            tag_(NONE)
            // , none_()
        {}

        Optional(const Some &v) noexcept:
            tag_(SOME),
            some_(v)
        {}

    public:
        bool is_some() const noexcept
        {
            return this->tag_ == SOME;
        }

        bool is_none() const noexcept
        {
            return this->tag_ == NONE;
        }

        const T &unwrap() const noexcept
        {
            if (!this->is_some()) std::terminate();
            return this->some_.unwrap();
        }

        T unwrap()  noexcept
        {
            if (!this->is_some()) std::terminate();
            return this->some_.unwrap();
        }

        const T &unwrap_or(const T &other) const noexcept
        {
            if (this->is_some()) return this->some_.unwrap();
            return other;
        }

        T unwrap_or(const T &other) noexcept
        {
            if (this->is_some()) return this->some_.unwrap();
            return other;
        }
};

}


#endif//NEL_OPTIONAL_HH
