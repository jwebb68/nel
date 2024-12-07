// -*- mode: c++; indent-tabs-mode: nil; tab-width: 4 -*-
#if !defined(NEL_SLICE_HH)
#    define NEL_SLICE_HH

namespace nel
{

template<typename T>
struct Slice;

template<typename T>
struct SliceIterator;

} // namespace nel

#    include <nel/iterator.hh>
#    include <nel/memory.hh>
// #    include <printio.hh>
#    include <nel/panic.hh>
#    include <nel/defs.hh>

#    include <cstddef> // std::nullptr_t

namespace nel
{

/**
 * Slice
 * models a block of contiguous ram.
 * Shared, multiple slices can point to the same ram region.
 * can be created from ptr+len or 2 ptrs (begin/end).
 * When slice is destroyed, the block if ram is not destroyed.
 * Items can be iterated over.
 * Items can be accessed via [] oper.
 * slice is empty if len is 0, regardless of ptr value.
 */

template<typename T>
struct Slice
{
    public:
        typedef T Type;

    private:
        Type *content_;
        // Implicit start at 0.
        Length len_;

    private:
        constexpr Slice(void)
            : content_(nullptr)
            , len_(0)
        {
        }

    public:
        // annoyingly, the type deducing form only works on ctors, not statics
        // so Slice( int ptr, len) gets a Slice<int>(int ptr, len)
        // but a Slice::from(int ptr, len) does not work..
        constexpr Slice(Type p[], Length len)
            : content_(p)
            , len_(len)
        {
        }

        constexpr Slice(Type *const b, Type *const e)
            : content_(b)
            , len_(e - b)
        {
        }

    public:
        // Copying a slice is ok as it does not own the data it points to.
        constexpr Slice(Slice const &) = default;
        constexpr Slice &operator=(Slice const &) = default;

    public:
        // moving a slice is ok.
        constexpr Slice(Slice &&o)
            : content_(move(o.content_))
            , len_(move(o.len_))
        {
            o.content_ = nullptr;
            o.len_ = 0;
        }

        constexpr Slice &operator=(Slice &&o)
        {
            if (this != &o) {
                content_ = move(o.content_);
                o.content_ = nullptr;
                len_ = move(o.len_);
                o.len_ = 0;
            }
            return *this;
        }

    public:
        /**
         * Create an empty slice.
         *
         * @return fresh empty slice
         */
        static constexpr Slice empty(void)
        {
            return Slice();
        }

    public:
        /**
         * Determine if the slice is empty, i.e. contains 0 elements.
         *
         * @returns true if slice is empty, false otherwise.
         */
        constexpr bool is_empty(void) const
        {
            return len() == 0;
        }

        constexpr operator bool() const
        {
            return !is_empty();
        }

        /**
         * Determine numer of elements in the slice.
         *
         * @returns number of elements in the slice.
         */
        constexpr Type *ptr(void) const
        {
            return content_;
        }

        /**
         * Determine numer of elements in the slice.
         *
         * @returns number of elements in the slice.
         */
        constexpr Length len(void) const
        {
            return len_;
        }

        /**
         * Return a ref to the value at index idx
         *
         * @param idx The index to access.
         *
         * index access is not checked for bounds overflow.
         * @warning UB if index is out-of-bounds.
         */
        constexpr Type &unchecked_get(Index idx) const
        {
            return content_[idx];
        }

        /**
         * Return a ref to the value at index idx
         *
         * @param idx The index to access.
         *
         * @warning Panic if index out-of-bounds.
         */
        constexpr Type &checked_get(Index idx) const
        {
            nel::panic_if_not(idx < len(), "nel::Slice<T>: index out of range");
            return unchecked_get(idx);
        }

        /**
         * Item access in slice.
         *
         * @param idx The index of the item to get.
         *
         * @returns reference to the item
         * @warning Will panic if idx is out-of-range for slice
         */
        constexpr Type &operator[](Index idx) const
        {
            return checked_get(idx);
        }

        /**
         * Return a mut reference to the value at idx or None.
         *
         * @param idx index of element to get
         *
         * @returns If array is empty, return None.
         * @returns If idx is out-of range, return None.
         * @returns else return ref to item at index..
         */
        constexpr Optional<Type &> try_get(Index idx) const
        {
            return (idx >= len_) ? None : Optional<Type &>::Some(content_[idx]);
        }

    public:
        /**
         * Compare two slices for equality.
         *
         * @param o the other slice to compare to.
         *
         * @return true if o is same ptr/len as this.
         * @return true if o is same len as this, points to diff location, and elems are the same
         * value.
         *
         * @warning UB if values cannot be read.
         */
        constexpr bool operator==(Slice const &o) const
        {
            return len() == o.len() && (ptr() == o.ptr() || elem::eq(ptr(), o.ptr(), len()));
        }

        /**
         * Compare two slices for inequality.
         *
         * @param o the other slice to compare to.
         *
         * @return true if o is same ptr/len as this.
         * @return true if o is same len as this, points to diff location, and elems are the same
         * value.
         *
         * @warning UB if values cannot be read.
         */
        constexpr bool operator!=(Slice const &o) const
        {
            return !(*this == o);
        }

    public:
        /**
         * Fill the slice with the value given.
         *
         * @param f value to use as a template.
         *
         * @warning panics if copying of f panics.
         */
        void fill(Type const &f)
        {
            elem::set(ptr(), f, len());
        }

        /**
         * Move contents of one slice to another of same length.
         *
         * @param src Slice to move values from.
         *
         * @note no effect if src is same as dest.
         * @warning panics if slices are of different lengths
         * @warning UB if dst refers to non-readable memory.
         * @warning UB if dst refers to non-writable memory.
         * @warning UB if src refers to non-readable memory.
         * @warning UB if src refers to non-writable memory.
         */
        // TODO: use try_move_from as operation can fail.
        void move_from(Slice &src)
        {
            nel::panic_if_not(len() == src.len(), "nel::Slice:move_from: Different lengths");
            elem::move(ptr(), src.ptr(), len());
        }

        /**
         * Copy contents of one slice to another of same length.
         *
         * @param src Slice to copy values from.
         *
         * @note no effect if src is same as dest.
         * @warning panics if slices are of different lengths
         * @warning UB if dst refers to non-readable memory.
         * @warning UB if dst refers to non-writable memory.
         * @warning UB if src refers to non-readable memory.
         */
        // TODO: use try_copy_from as operation can fail.
        void copy_from(Slice const &o)
        {
            nel::panic_if_not(len() == o.len(), "nel::Slice:copy_from: Different lengths");
            elem::copy(ptr(), o.ptr(), len());
        }

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
        Slice slice(Index b, Index e) const
        {
            if (b >= e) { return Slice::empty(); }
            if (b >= len_) { return Slice::empty(); }
            if (e > len_) { e = len_; }
            return Slice(&content_[b], e - b);
        }

    public:
        /**
         * Return an iterator that will iterate over the slice
         *
         * The iterator is invalidated if the slice goes out of scope/destroyed.
         */
        typedef SliceIterator<Type> Iterator;

        constexpr Iterator iter(void) const
        {
            return Iterator(ptr(), len());
        }

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
        friend Log &operator<<(Log &outs, Slice const &v)
        {
            outs << "Slice(" << v.len() << "){";
#    if defined(RUST_LIKE)
            auto it = v.iter();
            auto itv = it.next();
            if (itv.is_some()) {
                outs << itv.unwrap();
                it.for_each([&outs](T const &e) { outs << ' ' << e; });
            }
#    elif 0 && defined(C_LIKE)
            if (v.len() > 0) {
                outs << v.content_[0];
                for (Index i = 1; i < v.len(); ++i) {
                    outs << ' ' << v.content_[i];
                }
            }
#    elif defined(C_LIKE)
            auto it = v.iter();
            if (it) {
                outs << *it;
                ++it;
                it.for_each([&outs](T const &e) { outs << ' ' << e; });
            }
#    endif
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
class SliceIterator: public Iterator<SliceIterator<T>, T &, T &>
{
    public:
        typedef T &InT;
        typedef T &OutT;

    private:
        T *b_;
        T *e_;

    public:
        constexpr SliceIterator(void) = delete;

    public:
        // copy ok
        SliceIterator(SliceIterator const &o) = default;
        SliceIterator &operator=(SliceIterator const &o) = default;

    public:
        // move ok
        SliceIterator(SliceIterator &&o)
            : b_(move(o.b_))
            , e_(move(o.e_))
        {
            o.b_ = nullptr;
            o.e_ = nullptr;
        }

        SliceIterator &operator=(SliceIterator &&o)
        {
            if (this != &o) {
                b_ = move(o.b_);
                e_ = move(o.e_);
                o.b_ = nullptr;
                o.e_ = nullptr;
            }
            return *this;
        }

    public:
        constexpr SliceIterator(T arr[], Count len)
            : b_(arr)
            , e_(arr + len)
        {
        }

        constexpr SliceIterator(T *const b, T *const e)
            : b_(b)
            , e_(e)
        {
        }

#    if defined(RUST_LIKE)
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
        Optional<OutT> next(void)
        {
            // Some() takes a move, so want to move the reference into the optional.
            // ref(),
            if (b_ == e_) { return None; }
            return Some(*b_++);
        }
#    endif

    public:
#    if defined(C_LIKE)
        constexpr bool is_done(void) const
        {
            return (b_ == e_);
        }

        void inc(void)
        {
            ++b_;
        }

        OutT deref(void)
        {
            return *b_;
        }
#    endif
};

}; // namespace nel

#endif // defined(NEL_SLICE_HH)
