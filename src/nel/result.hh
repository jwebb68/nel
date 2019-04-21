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

#include <exception> //std::terminate


namespace nel
{
template<typename T>
class OkT
{
    private:
        T value_;

    public:
        OkT() noexcept
        {}

        explicit OkT(const T &ok) noexcept:
            value_(ok)
        {}

    public:
        // TODO: get_value or deref operator for value access?
        const T &get_value() const noexcept
        {
            return this->value_;
        }

        T get_value() noexcept
        {
            return this->value_;
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
        E value_;

    public:
        ErrT() noexcept
        {}

        explicit ErrT(const E &err) noexcept:
            value_(err)
        {}

    public:
        // TODO: get_value or deref operator for value access?
        const E &get_value() const noexcept
        {
            return this->value_;
        }

        E get_value() noexcept
        {
            return this->value_;
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
        } tag_;

        union
        {
            OkT<T> ok_;
            ErrT<E> err_;
        };

    public:
        ~ResultT() noexcept
        {
            switch (this->tag_)
            {
                case OK:
                    this->ok_.OkT<T>::~OkT();
                    break;

                case ERR:
                    this->err_.ErrT<E>::~ErrT();
                    break;

                default:
                    std::terminate();
                    break;
            }
        }

        ResultT(const ResultT<T, E> &other) noexcept:
            tag_(other.tag_)
        {
            switch (other.tag_)
            {
                case OK:
                    this->ok_ = other.ok_;
                    break;

                case ERR:
                    this->err_ = other.err_;
                    break;

                default:
                    std::terminate();
                    break;
            }
        }


        ResultT(const OkT<T> &ok) noexcept:
            tag_(OK),
            ok_(ok)
        {}

        ResultT(const ErrT<E> &err) noexcept:
            tag_(ERR),
            err_(err)
        {}

    public:
        bool operator==(const OkT<T> &other) const
        {
            if (this->tag_ != OK) return false;
            return this->ok_ == other;
        }

        bool operator!=(const OkT<T> &other) const
        {
            return !((*this) == other);
        }

        bool operator==(const ErrT<E> &other) const
        {
            if (this->tag_ != ERR) return false;
            return this->err_ == other;
        }

        bool operator!=(const ErrT<T> &other) const
        {
            return !((*this) == other);
        }

    public:
        bool is_ok() const noexcept
        {
            return this->tag_ == OK;
        }

        bool is_err() const noexcept
        {
            return this->tag_ == ERR;
        }

        OptionalT<T> ok() const noexcept
        {
            if (this->tag_ == OK) return this->ok_.get_value();
            return None();
        }

        OptionalT<E> err() const noexcept
        {
            if (this->tag_ == ERR) return this->err_.get_value();
            return None();
        }

        const T &unwrap() const noexcept
        {
            if (this->tag_ == OK) return this->ok_.get_value();
            std::terminate();
        }

        T unwrap() noexcept
        {
            if (this->tag_ == OK) return this->ok_.get_value();
            std::terminate();
        }

        const T &unwrap_err() const noexcept
        {
            if (this->tag_ == ERR) return this->err_.get_value();
            std::terminate();
        }

        T unwrap_err() noexcept
        {
            if (this->tag_ == ERR) return this->err_.get_value();
            std::terminate();
        }

        const T &unwrap_or(const T &other) const noexcept
        {
            if (this->tag_ == OK) return this->ok_.get_value();
            return other;
        }

        T unwrap_or(const T &other) noexcept
        {
            if (this->tag_ == OK) return this->ok_.get_value();
            return other;
        }
};

#define nel_result_try(r) {if (r.is_err()) return r;}while(0);

}


#endif//NEL_RESULT_HH
