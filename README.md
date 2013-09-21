solid - a minimalist language with a tiny VM ![Build Status] (https://travis-ci.org/chameco/Solid.png)
--------------------------------------------

Solid is a very small, interpreted, object-oriented language.

Documentation is currently nonexistant, but the code is pretty standard object-oriented C99 (generally one main struct per file, "methods" are functions that take a struct pointer as the first argument, everything is allocated with `malloc`). Start in ast.c and vm.c.

Functional stuff is coming, don't worry! Expect lambdas, tail calls, and lots of higher-order functions.
