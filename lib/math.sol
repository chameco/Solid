Math = ns {
	import("lib/_math.so");
	factorial = fn x {
		if x == 1 return 1;
		return x * this(x - 1);
	};
};
