#ifndef NEL_HEAPLESS_VECTOR_HH
#define NEL_HEAPLESS_VECTOR_HH

namespace nel {
namespace heapless {

template<typename T, size_t const N>
struct Vector;

}
}

#include "log.hh"
#include "iterator.hh"
#include "enumerator.hh"
#include "slice.hh"
#include "result.hh"
#include "optional.hh"

namespace nel {
namespace heapless {

template<typename T, size_t const N>
struct Vector {
    public:
    private:
        // number initialised
        size_t len_;
        // must create with N uninitialised
        // so really want only the memory allocated.
        // will manually initialise on push_back, deinitialise on pop_back/dtruct
        // C++: default array cons does not call default on each, but allocs uninit?
        T values_[N];

    public:
        ~Vector(void)
        {
            for (size_t i = 0; i < len(); ++i) {
                values_[i].~T();
            }
        }

        // no copying..
        constexpr Vector(Vector const &o) = delete;
        constexpr Vector &operator=(Vector const &o) = delete;

        // moving ok
        Vector(Vector &&o) noexcept
            : len_(std::move(o.len_))
        {
            o.len_ = 0;
            for (size_t i = 0;i < len(); ++i) {
                new (&values_[i]) T(std::move(o.values_[i]));
            }
        }
        Vector &operator=(Vector &&o) noexcept
        {
            // expensive to call swap on large inplace object.
            this->~Vector();
            new (this) Vector(std::move(o));
            return *this;
        }

        constexpr Vector(void)
            : len_(0)
        {}


        constexpr Vector(std::initializer_list<T> l)
        {
            size_t i = 0;
            for (auto it = l.begin(); i < capacity() && it != l.end(); ++it, ++i) {
                new (&values_[i]) T(std::move(*it));
            }
            len_ = i;
            // TODO: what if l.size() > capacity()? want to fail compile..
        }

    public:
        constexpr size_t capacity(void) const noexcept
        {
            return N;
        }
        constexpr size_t len(void) const noexcept
        {
            return len_;
        }
        constexpr bool is_empty(void) const noexcept
        {
            return len_ == 0;
        }
        void empty(void) noexcept
        {
            for (size_t i = 0; i < len(); ++i) {
                values_[i].~T();
            }
            len_ = 0;
        }

        constexpr T &operator[](size_t idx) noexcept
        {
            nel_panic_ifnot(idx < len(), "index out of range");
            return values_[idx];
        }
        constexpr T const &operator[](size_t idx) const noexcept
        {
            nel_panic_ifnot(idx < len(), "index out of range");
            return values_[idx];
        }


        // TODO: fail on fixed?
        // or return Result<void, ?>
        // and fail if over N
        // shouldn't be errT=bool but don't know what to use..
        //Result<void, bool> reserve(size_t new_capacity) noexcept
        void reserve(size_t new_capacity) noexcept
        {
            NEL_UNUSED(new_capacity);
        }

        template<typename ...Args>
        Result<void, T> push_back(Args &&...args) noexcept
        {
            if (len() >= capacity()) {
                // really? must one be created for err?
                return Result<void, T>::Err(std::forward<Args>(args)...);
            }
            // remember, values at len and beyond are uninitialised.
            // so need to use new to contruct them.
            new (&values_[len()]) T(std::forward<Args>(args)...);
            len_ += 1;
            return Result<void, T>::Ok();
        }

        Optional<T> pop_back(void) noexcept
        {
            if (len() == 0) {
                return Optional<T>::None();
            }

            auto v = Optional<T>::Some(std::move(values_[len()]));
            values_[len()].~T();
            len_ -= 1;
            return v;
        }

        constexpr Slice<T> slice(void) noexcept
        {
            return Slice<T>(values_, len());
        }
        constexpr Slice<T const> const slice(void) const noexcept
        {
            return Slice<T const>(values_, len());
        }


        constexpr Slice<T> slice(size_t b, size_t e) noexcept
        {
            return slice().subslice(b, e);
        }
        constexpr Slice<T const> slice(size_t b, size_t e) const noexcept
        {
            return slice().subslice(b, e);
        }


        constexpr Iterator<T> iter(void) noexcept
        {
            return slice().iter();
        }
        constexpr Iterator<T const> iter(void) const noexcept
        {
            return slice().iter();
        }

        constexpr Enumerator<T> enumer(void) noexcept
        {
            return slice().enumer();
        }
        constexpr Enumerator<T const> enumer(void) const noexcept
        {
            return slice().enumer();
        }

    public:
        friend Log &operator<<(Log &outs, Vector const &v) noexcept
        {
            outs << "Vector<" << N << ">("<< v.len() <<"){" << "\n";
            for (size_t i=0; i < v.len(); ++i) {
                outs << "["<<i<<"]:" << v.values_[i] << "\n";
            }
            outs << "}";
            return outs;
        }
};

}
}



#endif//NEL_HEAPLESS_VECTOR_HH
