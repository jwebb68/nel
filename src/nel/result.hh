#ifndef NEL_RESULT_HH
#define NEL_RESULT_HH

namespace nel
{

template<typename T>
class OkT;

template<typename E>
class ErrT;

template<typename T, typename E>
class ResultT;

}

#include "optional.hh" //nel::OptionalT

#include <cassert> // assert()
#include <exception> //std::terminate


namespace nel
{
template<typename T>
class OkT
{
    private:
        T value;
    public:
        OkT() noexcept
        {}
        explicit OkT(const T &other) noexcept:
            value(other)
        {}
    public:
        const T &getvalue() const noexcept
        {
            return this->value;
        }
        T getvalue() noexcept
        {
            return this->value;
        }
};
template<typename T>
OkT<T> Ok(const T &v) noexcept
{
    return OkT<T>(v);
}


template<typename E>
class ErrT
{
    private:
        E value;
    public:
        ErrT() noexcept
        {}
        explicit ErrT(const E &other) noexcept:
            value(other)
        {}
    public:
        const E &getvalue() const noexcept
        {
            return this->value;
        }
        E getvalue() noexcept
        {
            return this->value;
        }
};
template<typename E>
ErrT<E> Err(const E &v) noexcept
{
    return ErrT<E>(v);
}


template<typename T, typename E>
class ResultT
{
    private:
        enum
        {
            OK = 0,
            ERR
        } tag;
        union
        {
            OkT<T> ok_;
            ErrT<E> err_;
        };

    public:
        ~ResultT() noexcept
        {
            switch (this->tag)
            {
                case OK:
                    this->ok_.OkT<T>::~OkT();
                    break;

                case ERR:
                    this->err_.ErrT<E>::~ErrT();
                    break;

                default:
                    assert(false);
                    break;
            }
        }

        ResultT(const ResultT<T, E> &other) noexcept:
            tag(other.tag)
        {
            switch (other.tag)
            {
                case OK:
                    this->ok_ = other.ok_;
                    break;

                case ERR:
                    this->err_ = other.err_;
                    break;

                default:
                    assert(false);
                    break;
            }
        }


        ResultT(const OkT<T> &ok) noexcept:
            tag(OK),
            ok_(ok)
        {}

        ResultT(const ErrT<E> &err) noexcept:
            tag(ERR),
            err_(err)
        {}


    public:
        bool operator==(const OkT<T> &other) const
        {
            if (this->tag != OK) return false;
            return this->ok_ == other;
        }
        bool operator!=(const OkT<T> &other) const
        {
            return !((*this) == other);
        }

        bool operator==(const ErrT<E> &other) const
        {
            if (this->tag != ERR) return false;
            return this->err_ == other;
        }
        bool operator!=(const ErrT<T> &other) const
        {
            return !((*this) == other);
        }

    public:
        bool is_ok() const noexcept
        {
            return this->tag == OK;
        }
        bool is_err() const noexcept
        {
            return this->tag == ERR;
        }

        OptionalT<T> ok() const noexcept
        {
            if (this->tag == OK) return this->ok_.getvalue();
            return None();
        }
        OptionalT<E> err() const noexcept
        {
            if (this->tag == ERR) return this->err_.getvalue();
            return None();
        }

        const T &unwrap() const noexcept
        {
            if (this->tag == OK) return this->ok_.getvalue();
            std::terminate();
        }
        T unwrap() noexcept
        {
            if (this->tag == OK) return this->ok_.getvalue();
            std::terminate();
        }

        const T &unwrap_err() const noexcept
        {
            if (this->tag == ERR) return this->err_.getvalue();
            std::terminate();
        }
        T unwrap_err() noexcept
        {
            if (this->tag == ERR) return this->err_.getvalue();
            std::terminate();
        }

        const T &unwrap_or(const T &other) const noexcept
        {
            if (this->tag == OK) return this->ok_.getvalue();
            return other;
        }

        T unwrap_or(const T &other) noexcept
        {
            if (this->tag == OK) return this->ok_.getvalue();
            return other;
        }

};

#define nel_result_try(r) {if (r.is_err()) return r;}while(0);

}


#endif//NEL_RESULT_HH
