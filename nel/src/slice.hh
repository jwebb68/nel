#ifndef NEL_HEAPLESS_SLICE_HH
#define NEL_HEAPLESS_SLICE_HH

namespace nel {

template<typename T>
struct Slice;

} // namespace nel

#include "iterator.hh"
#include "enumerator.hh"
#include "panic.hh"

namespace nel {

template<typename T>
struct Slice {
    private:
        T *const content_;
        // Implicit start at 0.
        size_t len_;

    public:
        constexpr Slice(T p[], size_t len) noexcept
            : content_(p)
            , len_(len)
        {}

    public:
        Slice(void) = delete;

        // Could copy.
        Slice(Slice const &) = delete;
        Slice &operator=(Slice const &) const = delete;

        Slice(Slice &&o) noexcept = default;
        Slice &operator=(Slice &&o) noexcept = default;

    public:
        constexpr size_t len(void) const noexcept
        {
            return len_;
        }


        constexpr T const &operator[](size_t idx) const noexcept
        {
            nel_panic_if_not(idx < len(), "index out of range");
            return content_[idx];
        }


        constexpr T &operator[](size_t idx) noexcept
        {
            nel_panic_if_not(idx < len(), "index out of range");
            return content_[idx];
        }


        void fill(T const &f) noexcept
        {
            nel::memset(content_, f, len());
        }


        constexpr Slice subslice(size_t b, size_t e) noexcept
        {
            nel_panic_if(b > e, "index swapped");
            nel_panic_if(b > len_, "b index out of range");
            nel_panic_if(e > len_, "e index out of range");
            return Slice(content_ + b, e - b);
        }


        constexpr Slice const subslice(size_t b, size_t e) const noexcept
        {
            nel_panic_if(b > e, "index swapped");
            nel_panic_if(b > len_, "b index out of range");
            nel_panic_if(e > len_, "e index out of range");
            return Slice(content_ + b, e - b);
        }


        void copy_from(Slice const &o) noexcept
        {
            nel_panic_if(len() != o.len(), "not same size");
            nel::memcpy(content_, o.content_, len());
        }


        void move_from(Slice &o) noexcept
        {
            nel_panic_if(len() != o.len(), "not same size");
            nel::memmove(content_, o.content_, len());
        }


        constexpr Iterator<T> iter(void) noexcept
        {
            return Iterator<T>(content_, len_);
        }


        constexpr Iterator<T const> const iter(void) const noexcept
        {
            return Iterator<T const>(content_, len_);
        }


        constexpr Enumerator<T> enumerate(void) noexcept
        {
            return Enumerator<T>(content_, len_);
        }


        constexpr Enumerator<T const> const enumerate(void) const noexcept
        {
            return Enumerator<T const>(content_, len_);
        }


    public:
        friend Log &operator<<(Log &outs, Slice const &v) noexcept
        {
            outs << "Slice(" << v.len() << "){";
            if (v.len() > 0) {
                outs << v.content_[0];
                for (size_t i = 1; i < v.len(); ++i) {
                    outs << " " << v.content_[i];
                }
            }
            outs << "}";
            return outs;
        }
};

} // namespace nel


#endif // NEL_HEAPLESS_SLICE_HH
