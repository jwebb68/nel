NEL
===
or No Exceptions Library - NEL.

A library to help develop C++ applications to not use exceptions,
e.g. embedded.

Written in the rust style.
It's also a research area to investigate if rust-isms can translate to c++.

Objectives
----------
1. A return mechanism that does not use exceptions.
2. That is more powerful than simple int return codes.
3. Has zero-cost abstractions.
4. Easy to use.
5. Simple to understand.

Reasons
-------
1. Exceptions are slow in their unwinding phase - depending on
   implementation.
2. They add another implicit execution path to the code.
3. On throwing, and depending on implementation, an implicit heap
   allocation is done (heap may not be available or may be exhausted).

Reasons not to
--------------
1. yet another library, is another needed (arguably no).
2. why re-invent wheels?
3. C++ is not rust, don't attempt to do in c++ what rust does as it prob wont work that way.

TODO
====
1. add documentation: doxygen? other?
2. revamp makefile: is there a better build sys than make? (not
   cmake), e.g. something like rust's cargo ?
3. add proper tests, using decent test lib/framework (boost? catch2?
   google?)
4. add install target to install into system..?


Design patterns
----------------
1. pass by ref (const/mut), pass by owned (move), never pass by-value (implicit copy)
2. if object is passed in by move and error occurs, consider passing back out as the error in a result.
3. avoid use of Heap , although some types will use it internally (String/Box/RC ).
4. have separate types for things like size/len and indexes (all are impl as size_t, but should be distinct types).
  -  prevents use of an index as a size/len unless explicitly converted.
5. as not using exceptions then returning errors in a constructor is impossible.
   - so don't use constructors, use creator/factory funcs.
   - e.g. not String x("oh noes"); but String x = String::try_from("oh noes").unwrap();
6. T T::from(U &&) and U T::into<U>() MUST consume the value the source value.
7. C++ is not Rust, so adding new conversions is hard as won't appear as dot notation.
8. no implicit copying - consider disabling copy-cons for all types - use explicit .copy() func to copy.
9. use operators? *, ++v, v[idx] etc.. save typing..?


naming notes:
.len = number of items in string/collection
.size = number of bytes used (cf sizeof)
.capacity = max number of items allowed.
[.resize = increase max number of items allowed]

prevent len/size being used as an index:
typedef size_t Size; ?
typedef size_t Index; ?
then:
struct Len {...};
struct Size {...};
- can size be -ve ?
- can index be -ve
- IndexDiff can be -ve
- SizeDiff can be -ve

String<T>:
- heapless/heaped
- heaped: COW? ZC?
- heaped: overalloc?
- diff between this and vector?

Char (unicode32 internally?)?, not char
- multi-encoding
- to/from octets?

char maps to Char<ansi>?
-String<char> from char const *? (memcpy)
-String<char> as char const *? (ptr cast)
-String<T> == Array<T>? what is diff between string and array

Error:
- how to handle errors,
- how to report errors..
- conversion between error types (auto?)
- custom/ud errors?

Formatting (to String<char>):
- debug formatting
- non debug formatting

io::Stream:
- is-good, is-bad, is-eof (eos?)
- typedef uint8_t Octet?
- writing: Result<size_t, ?> write(Slice<uint8_t const> &src)
- reading: Result<size_t, ?> read(Slice<uint8_t> &src)
 (write/read on Vector, read/write on array, on slice ?)

io::[de]serialisation
- T to octets
- octets to T

MemoryFile (in memory file, unbuffered)
File (unbuffered)
Buffer (buffers for file)
IOErrors

sockets (blocking)
TCP (stream)
UDP ?
socket errors? ioerror?

threads
- ARC?
- threadpool

threadsync/message-passing:
- queue inf depth
- queue finite depth
- sync point/channel: queue(depth=0),
-
