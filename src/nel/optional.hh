#ifndef NEL_OPTIONAL_HH
#define NEL_OPTIONAL_HH

namespace nel
{

template<typename T>
class SomeT;

template<typename T>
class OptionalT;

}


#include "none.hh"

#include <exception> //std::terminate

namespace nel
{

template<typename T>
class SomeT
{
    private:
        T value_;

    public:
        explicit SomeT(const T &other) noexcept:
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
template<typename T>
SomeT<T> Some(const T &other) noexcept
{
    return SomeT<T>(other);
}


template<typename T>
class OptionalT
{
    private:
        enum
        {
            NONE = 0,
            SOME
        } tag_;

        union
        {
            SomeT<T> some_;
            NoneT none_;
        };

    public:
        ~OptionalT() noexcept
        {
            switch (this->tag_)
            {
                case SOME:
                    this->some_.SomeT<T>::~SomeT();
                    break;

                case NONE:
                    this->none_.NoneT::~NoneT();
                    break;

                default:
                    std::terminate();
                    break;
            }
        }

        OptionalT(const OptionalT<T> &other) noexcept:
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

        OptionalT() noexcept:
            tag_(NONE),
            none_()
        {}

        OptionalT(const NoneT &) noexcept:
            tag_(NONE),
            none_()
        {}

        OptionalT(const SomeT<T> &v) noexcept:
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
template<typename T>
OptionalT<T> Optional(const T &other) noexcept
{
    return OptionalT<T>(other);
}

}


#endif//NEL_OPTIONAL_HH
