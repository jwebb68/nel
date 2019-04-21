#ifndef NEL_OPTIONAL_HH
#define NEL_OPTIONAL_HH

namespace nel
{

template<typename T>
class Some;

template<typename T>
class Optional;

}


#include "none.hh"

#include <exception> //std::terminate

namespace nel
{

template<typename T>
class Some
{
    private:
        T value_;

    public:
        // template arg deduction came in in c++17
        // so for earlier, need a make_ func.
        // or use c++17 or later.
        explicit Some(const T &val) noexcept:
            value_(val)
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


template<typename T>
class Optional
{
    private:
        enum
        {
            NONE,
            SOME
        } tag_;

        union
        {
            Some<T> some_;
            None none_;
        };

    public:
        ~Optional() noexcept
        {
            switch (this->tag_)
            {
                case SOME:
                    this->some_.Some<T>::~Some();
                    break;

                case NONE:
                    this->none_.None::~None();
                    break;

                default:
                    std::terminate();
                    break;
            }
        }

        Optional(const Optional<T> &other) noexcept:
            tag_(other.tag_)
        {
            switch (other.tag_)
            {
                case SOME:
                    this->some_ = other.some_;
                    break;

                case NONE:
                    this->none_ = other.none_;
                    break;

                default:
                    std::terminate();
                    break;
            }
        }

        Optional() noexcept:
            tag_(NONE),
            none_()
        {}

        Optional(const None &) noexcept:
            tag_(NONE),
            none_()
        {}

        Optional(const Some<T> &v) noexcept:
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
            if (this->tag_ == SOME) return this->some_.unwrap();
            std::terminate();
        }

        T unwrap()  noexcept
        {
            if (this->tag_ == SOME) return this->some_.unwrap();
            std::terminate();
        }

        const T &unwrap_or(const T &other) const noexcept
        {
            if (this->tag_ == SOME) return this->some_.unwrap();
            return other;
        }

        T unwrap_or(const T &other) noexcept
        {
            if (this->tag_ == SOME) return this->some_.unwrap();
            return other;
        }
};

}


#endif//NEL_OPTIONAL_HH
