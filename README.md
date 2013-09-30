solid - a minimalist language with a tiny VM ![Build Status] (https://travis-ci.org/chameco/Solid.png)
============================================

Solid is a simple, elegant language with a super simple C API. Yes, that means you can embed it into your application or game with almost no effort.

Installation
-------------

You'll want Bison (3.0.0), Flex (2.5), and a C compiler.

The Makefile should automagically fetch other dependencies and stuff. Just do:

    git clone http://github.com/chameco/Solid
    cd Solid
    make && sudo make install

Usage
------
Running a file is simple: `solid test.sol`.
To start a REPL (which is currently really bad, and doesn't have line continuation stuff), just run `solid`.

Syntax
------
Assignment: `x = 1`.

Basic math: `1 + 2`, or since operators are just functions, `+(1, 2)`.

Branching: `if 1 == 1 print("hello there")`.

Looping: `while 1 == 1 print("solid is still the best")`.

Blocks: Anywhere you can have a single expression, you can have a block. Blocks start with either `do` or `{`, and end with `end` or `}`. There are no formal rules about which to use, but I've taken to using `do` and `end` on multi-line blocks, and curlies for one-liners. Examples:

    c = 0
    while c < 10 do
    	print(c)
    	c = c + 1
    end

    c = 0
    while c < 10 {print(c); c = c + 1;}

Oh, and semicolons are automatically inserted by the lexer in the same manner as Go. Just follow Python logic about them and you'll be fine.

Functions: You can get an anonymous function like so: `fn a a * a`. But wait, how am I supposed to call it? Solid is designed so there is only one core language element that has an intrinsic side effect: assignment. Obviously we've been using `print`, but that's not in the language core, rather it's a function written in C that works just like a normal function. More on that later, but now, let's define a longer function.

    f = fn a do
    	print(a)
    	a * a
    end
    
    print(f(10))

Additionally, functions are all full closures. The classic example:

    make_counter = fn do
        counter = 0
        return fn do
            counter = counter + 1
        end
    end
    
    c1 = make_counter()
    c2 = make_counter()
    print(c1())
    print(c2())
    print(c1())
    print(c2())

Recursion: Since all functions are nameless, and the only method of assignment is `<identifier> = <expression>`, recursion is possible through a `this` variable saved inside the function's closure. You'll see in the next example.

Inline functions: If a function only includes symbols in its name, you can call it inline. To derive from the previous example, consider the following.

    ^ = fn a, b do
        if b == 0 return 1
        a * this(a, b - 1)
    end
    
    print(10 ^ 2)

Notice the recursion via use of `this`.

Namespaces: A namespace is pretty much a hash table or object.

    Math = ns do
        ^ = fn a, b do
            if b == 0 return 1
            a * a ^ b - 1
        end
    end
    
    print(Math.^(10, 2))
    ^ = Math.^;
    print(10 ^ 2)

A complete object system based on cloning namespaces is in the works, but right now feel free to call `clone` with a namespace argument to both derive classes and make instances. Don't worry about the overhead of having copies of functions in instances, as functions are represented as pointers internally.

Lists: Make linked lists with the following syntax: `x = ["a", "b", "c", 1, 2, 3]`. Index them like so: `x !! 1`, which would evaluate to `"b"`. Lists are immutable, but you can add items with the cons operator, `:`. Another example:

    x = [2, 3, 4]
    print(x !! 0)
    y = 1 : x
    print(y !! 0)

Dynamic Evaluation:
 * `import` is a pretty important standard library function. Call `import("filename")` to load a file. Calling `import` on a file with a `.sol` extension will just execute whatever solid code is in that file in the current namespace, returning the result. However, calling it on a shared library (`.so`, not `.so.1`) will invoke the foreign function interface. More on that in the next section.

And for now, that's pretty much it. My next milestones are pattern matching and garbage collection, but feel free to suggest things you'd like to see in the language by raising GitHub issues.

Foreign Function Interface and API
-----------------------------------
Now this is where it gets interesting. Solid exposes a complete C API that allows for incredibly easy embedding. All you really need to know:
 * To use the API, include `<solid/solid.h>` and link with `-lsolid`.
 * Parse a file or expression into an AST with `solid_parse_file(<path>)` and `solid_parse_expr(<expr>`. Example: `solid_ast_node *n = solid_parse_expr("1 + 1");`
 * Compile an AST into a function with `solid_parse_tree(<ast_node>)`. Example: `solid_object *func = solid_parse_tree(solid_parse_expr("1 + 1"));`
 * Make a virtual machine with `solid_make_vm()`. Example: `solid_vm *vm = solid_make_vm();`
 * Run code on a VM. Example: `solid_call_func(vm, func);`
 * Any expression you evaluate in solid puts the result in the return register, accesible at `vm->regs[255]`. However, this value will be of type `solid_object *`. To convert to C types, use `solid_get_str_value(<object>)`, `solid_get_int_value(<object>)`, and `solid_get_bool_value(<object>)`.
 * Convert C primitives to solid objects with `solid_str(<string>)`, `solid_int(<integer>)`, and `solid_bool(<integer>)`.
 * Use namespaces with `solid_get_namespace(<namespace>, <solid_string>)` and `solid_set_namespace(<namespace>, <solid_string>, <object>)`. You can get the global namespace by calling `solid_get_current_namespace(<vm>)`.
 * Turn a C function with declaration `void <function>(solid_vm *vm)` into a callable solid object function with `solid_define_c_function(<function>)`. You can access arguments by popping the VM stack (you'll get them in reverse order) with `solid_pop_stack(<vm>)`, and return a value by setting `vm->regs[255]`.

To put it all together, here's a complete example of embedding solid into a C program:

    #include <solid/solid.h>
    #include <stdio.h>
    
    void hello_world(solid_vm *vm)
    {
        solid_object *argument = solid_pop_stack(vm);
        printf("Howdy, %s!\n", solid_get_str_value(argument));
        vm->regs[255] = solid_int(1336)
    }
    int main()
    {
        solid_vm *vm = solid_make_vm();
        solid_object *compiled_expr = solid_parse_tree(solid_parse_expr("1 + my_function()"));
        solid_set_namespace(solid_get_current_namespace(vm), solid_str("my_function"), solid_define_cfunc(hello_world));
        solid_call_func(vm, compiled_expr);
        printf("solid is super %d", solid_get_int_value(vm->regs[255]));
    }


But wait, what if you want to use C from inside solid, rather than solid from inside C? Well, you do (almost) the exact same thing. The `import` function is capable of loading shared libraries with the extension `.so`. When loaded, solid will call an arbitrary, user-defined function named `solid_init` with the signature `void solid_init(solid_vm *vm)` inside the library, passing it the current VM. From there, you can do everything that we did above, defining functions, modifying namespaces, etc. Don't want to go through the trouble to manually build a shared library? Solid has you covered. Just throw your C file `whatever.c` containing `solid_init` in the `lib` folder of the solid source tree, and run `make lib TARGET=whatever`, and it will create a shared library called `whatever.so` in the solid source root, which can now be freely imported.

Contributing
-------------
Documentation is currently nonexistent, but the code is pretty standard object-oriented C99 (generally one main struct per file, "methods" are functions that take a struct pointer as the first argument, everything is allocated with `malloc`). Start in ast.c and vm.c.

License
--------
Solid is distributed under the MIT license. See LICENSE.md.
