solid - a minimalist language with a tiny VM
--------------------------------------------
Solid is a very small, interpreted, object-oriented language.

It has some nice things (or bad things, depending on your perspective):
 - A different sort of object model in comparision most languages
 - A bytecode compiler. The bytecode is run by the aforementioned tiny VM (it really is tiny, it's less than a KSLOC at the moment)
 - The VM is reentrant and uses no global variables, and the same holds true for the parser and lexer, so embedding should be easy.
 - Immutable strings

Functional stuff is coming, don't worry! Expect lambdas, tail calls, and lots of higher-order functions. I say functional stuff and not Haskell stuff for a reason. Learn a Lisp and you'll know what I mean.

Right now, only the VM is complete: feel free to check it out. The parser/compiler is under heavy development, and is only half implemented at the moment.
