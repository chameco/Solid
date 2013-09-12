attr = 20;
foo = class () {
	attr = 10;
	meth = fn (x) { attr = attr + x; };
};

meth = foo.meth;
bar = new foo;
meth2 = bar.meth;

foo.attr = 200;
x = foo.attr;
y = bar.attr;
