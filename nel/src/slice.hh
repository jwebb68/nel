#ifndef NEL_HEAPLESS_SLICE_HH
#define NEL_HEAPLESS_SLICE_HH

namespace nel
{

template<typename T>
struct Slice;

} // namespace nel

#include "iterator.hh"
#include "enumerator.hh"
#include "panic.hh"
#include "memory.hh"

namespace nel
{

template<typename T>
struct Slice {
    private:
        T *const content_;
        // Implicit start at 0.
        size_t len_;

    protected:
        constexpr Slice(void): content_(nullptr), len_(0) {}

        constexpr Slice(T p[], size_t len) noexcept: content_(p), len_(len) {}

    public:
        /**
         * Create an empty slice.
         */
        static constexpr Slice empty(void) noexcept
        {
            return Slice();
        }

        /**
         * Create a slice over the carray and len given.
         *
         * Slice is invalidated if p goes out of scope or is deleted/destroyed.
         */
        static constexpr Slice from(T p[], size_t len) noexcept
        {
            return Slice(p, len);
        }

    public:
        // Copying a slice is ok as it does not own the data it points to.
        // Slice(Slice const &) noexcept = delete;
        // Slice &operator=(Slice const &) noexcept = delete;
        Slice(Slice const &) noexcept = default;
        Slice &operator=(Slice const &) noexcept = default;

        // moving a slice is ok.
        Slice(Slice &&o) noexcept = default;
        Slice &operator=(Slice &&o) noexcept = default;

    public:
        /**
         * Determine if the slice is empty, i.e. contains 0 elements.
         *
         * @returns true if slice is empty, false otherwise.
         */
        constexpr bool is_empty(void) const noexcept
        {
            return len() == 0;
        }

        /**
         * Determine numer of elements in the slice.
         *
         * @returns number of elements in the slice.
         */
        constexpr size_t len(void) const noexcept
        {
            return len_;
        }

        /**
         * Item access in slice.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item
         * @warning Will panic if idx is out-of-range for slice
         */
        // TODO: use try_get as index access can fail.
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

    public:
        /**
         * fill the slice with the value given.
         */
        void fill(T const &f) noexcept
        {
            nel::memset(content_, f, len());
        }

        // TODO: use try_copy_from as operation can fail.
        // void copy_from(Slice const &o) noexcept
        // {
        //     nel_panic_if(len() != o.len(), "not same size");
        //     nel::memcpy(content_, o.content_, len());
        // }

        // TODO: use try_move_from as operation can fail.
        // void move_from(Slice &o) noexcept
        // {
        //     nel_panic_if(len() != o.len(), "not same size");
        //     nel::memmove(content_, o.content_, len());
        // }

    public:
        /**
         * Return an iterator that will iterate over the slice
         *
         * The iterator is invalidated if the slice goes out of scope/destroyed.
         */
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
