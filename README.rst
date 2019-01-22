NEL
===
or No Exceptions Library - NEL.

A library to help develop C++ applications to not use exceptions,
e.g. embedded.


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


TODO
====
1. add documentation: doxygen? other?
2. revamp makefile: is there a better build sys than make? (not
   cmake), e.g. something like rusts's cargo ?
3. add proper tests, using decent test lib/framework (boost? catch2?
   google?)
4. add install target to install into system..?
