fib_recursive = fn n {
	if <(n, 2) return n;
	return $(n - 1) + $(n - 2);
};

print(fib_recursive(20));
