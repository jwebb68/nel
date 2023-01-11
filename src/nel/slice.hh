#ifndef NEL_HEAPLESS_SLICE_HH
#define NEL_HEAPLESS_SLICE_HH

namespace nel
{

template<typename T>
struct Slice;

template<typename T>
struct SliceIterator;

} // namespace nel

#include <nel/iterator.hh>
#include <nel/memory.hh>
#include <nel/panic.hh>
#include <nel/defs.hh>

namespace nel
{

/**
 * A contiguous region
 */
template<typename T>
struct Slice {
    private:
        T *const content_;
        // Implicit start at 0.
        Length len_;

    private:
        constexpr Slice(void)
            : content_(nullptr)
            , len_(0)
        {
        }

        constexpr Slice(T p[], Length len) noexcept
            : content_(p)
            , len_(len)
        {
        }

    public:
        // Copying a slice is ok as it does not own the data it points to.
        constexpr Slice(Slice const &) noexcept = default;
        constexpr Slice &operator=(Slice const &) noexcept = default;

        // moving a slice is ok.
        constexpr Slice(Slice &&o) noexcept = default;
        constexpr Slice &operator=(Slice &&o) noexcept = default;

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
        static constexpr Slice from(T p[], Length len) noexcept
        {
            return Slice(p, len);
        }

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
        constexpr Length len(void) const noexcept
        {
            return len_;
        }

#if 0
        /**
         * Item access in slice.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item
         * @warning Will panic if idx is out-of-range for slice
         */
        // TODO: use try_get as index access can fail.
        constexpr T &operator[](Index idx) noexcept
        {
            nel_panic_if_not(idx < len(), "index out of range");
            return content_[idx];
        }
        constexpr T const &operator[](Index idx) const noexcept
        {
            nel_panic_if_not(idx < len(), "index out of range");
            return content_[idx];
        }
#endif

        /**
         * Return a reference to the value at idx or None.
         *
         * @param idx index of element to get
         *
         * @returns If array is empty, return None.
         * @returns If idx is out-of range, return None.
         * @returns else return ref to item at index..
         */
        constexpr Optional<T &> try_get(Index idx) noexcept
        {
            return (idx >= len_) ? None : Some(content_[idx]);
        }

        constexpr Optional<T const &> try_get(Index idx) const noexcept
        {
            return (idx >= len_) ? None : Optional<T const &>::Some(content_[idx]);
        }

    public:
        /**
         * fill the slice with the value given.
         *
         * T must be bitcopyable (i.e. not need special copy semantics.
         * value at each location is not destroyed.
         */
        void fill(T const &f) noexcept
        {
            nel::memset(content_, f, len());
        }

        // try_fill as creational
        // if fails, leaves slice in inconsistent state (partially created/initialised)
        // try_fill as post initied
        // just wastes cycles destroying things.

        /**
         * Get a partial slice over the range of elements in the slice.
         *
         * @param b the start index of the range to slice.
         * @param e the end index of the range to slice.
         *
         * @returns if array is empty, return empty slice
         * @returns if b&e >= array len, return empty slice
         * @returns if b >= array len, return empty slice
         * @returns if e > array len, clamp to last elem.
         * @returns else return slice over region b..e of slice.
         */
        Slice<T> subslice(Index b, Index e) noexcept
        {
            if (b >= e) { return Slice<T>::empty(); }
            if (b >= len_) { return Slice<T>::empty(); }
            if (e > len_) { e = len_; }
            return Slice(&content_[b], e - b);
        }

        Slice<T const> subslice(Index b, Index e) const noexcept
        {
            if (b >= e) { return Slice<T const>::empty(); }
            if (b >= len_) { return Slice<T const>::empty(); }
            if (e > len_) { e = len_; }
            return Slice(&content_[b], e - b);
        }

    public:
        // TODO: use try_copy_from as operation can fail.
        // Result<void, ??> try_copy_from(Slice const &o) noexcept ?
        // Optional<void> try_copy_from(Slice const &o) noexcept ?
        // void copy_from(Slice const &o) noexcept
        // {
        //     nel_panic_if(len() != o.len(), "not same size");
        //     nel::memcpy(content_, o.content_, len());
        // }

        // TODO: use try_move_from as operation can fail.
        // Result<void, ??> try_move_from(Slice &o) noexcept ?
        // Optional<void> try_move_from(Slice &o) noexcept ?
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
#if 0
        constexpr Iterator<T> iter(void) noexcept
        {
            return Iterator<T>(content_, len_);
        }
        constexpr Iterator<T const> const iter(void) const noexcept
        {
            return Iterator<T const>(content_, len_);
        }
#else
        constexpr SliceIterator<T> iter(void) noexcept
        {
            return SliceIterator<T>(content_, len());
        }

        constexpr SliceIterator<T const> const iter(void) const noexcept
        {
            return SliceIterator<T const>(content_, len());
        }
#endif

    public:
        /**
         * Format/emit a representation of this object as a charstring
         * for debugging purposes.
         *
         * @param val the value to format
         * @param outs the stream to dump the representation into.
         */
        // TODO: replace <<(Log ) with dbgfmt, so separate out from
        // any other form of conversion to charstring.
        // TODO: insert into formatter and not final dest type.
        // instead of insert into log, can it format into ? which log implements?
        // so it doesn't matter about the destination..
        // and can format-insert into any char endpoint.
        friend Log &operator<<(Log &outs, Slice const &v) noexcept
        {
            outs << "Slice(" << v.len() << "){";
            if (v.len() > 0) {
#if 0
                auto it = v.iter();
                auto itv = it.next();
                if (itv.is_none()) { goto exit; }
                outs << itv.unwrap();
                it.for_each([&outs](T const & e) { outs << ' ' << e;})
exit:
#else
                outs << v.content_[0];
                for (Index i = 1; i < v.len(); ++i) {
                    outs << ' ' << v.content_[i];
                }
#endif
            }
            outs << '}';
            return outs;
        }
};

/**
 * An Iterator over a range of T
 *
 * Iterator does not own what it's iterating over, so is invalidated if that goes out of scope.
 * Cannot be reset once exhausted.
 * Is non-consuming, items are not moved out of original container.
 */
// TODO: maybe make length a template param..
template<typename T>
class SliceIterator: public Iterator<SliceIterator<T>, T, T &>
{
    public:
    private:
        T *const ptr_;
        Count const len_;
        Index pos_;

    public:
        constexpr SliceIterator(void) = delete;

        // copy ok
        // move ok
    public:
        constexpr SliceIterator(T arr[], Count len) noexcept
            : ptr_(arr)
            , len_(len)
            , pos_(0)
        {
        }

        /**
         * Return next item in iterator or None is no more.
         *
         * @returns Optional::Some if iterator still active.
         * @returns Optional::None if iterator exhausted.
         */
        // if T == U & then mutating, non-consuming.
        // if T == U && then owning/consuming (val moved out of underlying collection)
        // if T == U then coping.. want to avoid.
        // mutating.non-consuming
        // non-mutating.non-consuming
        Optional<T &> next(void) noexcept
        {
            // Some() takes a move, so want to move the reference into the optional.
            // ref(),
            if (pos_ >= len_) { return None; }
            return Some(ptr_[pos_++]);
        }

        // consuming/moving/mutating iter
        // Optional<T> next(void) noexcept
        // {
        //     return (pos_ < len_) ? Optional<T>::Some(std::move(ptr_[pos_++])) :
        //     Optional<T>::None();
        // }
};
} // namespace nel

#endif // NEL_HEAPLESS_SLICE_HH
