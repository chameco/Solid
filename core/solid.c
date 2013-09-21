#include "solid.h"

int yyparse(ast_node **expression, yyscan_t scanner);

void d_type(solid_vm *vm, char *name)
{
	debug("%s: type of %d", name, get_namespace(get_current_namespace(vm), solid_str(name))->type);
}

void d_int(solid_vm *vm, char *name)
{
	debug("%s: %d, type of %d", name, get_int_value(get_namespace(get_current_namespace(vm), solid_str(name))), get_namespace(get_current_namespace(vm), solid_str(name))->type);
}

ast_node *parse_expr(char *expr)
{
	ast_node *ret = NULL;
	yyscan_t scanner;
	YY_BUFFER_STATE state;

	scanner_state scan_state = {.insert = 0};
	yylex_init_extra(&scan_state, &scanner);
	state = yy_scan_string(expr, scanner);
	yyparse(&ret, scanner);
	yy_delete_buffer(state, scanner);
	yylex_destroy(scanner);

	return ret;
}

ast_node *parse_file(char *path)
{
	FILE *f = fopen(path, "r");
	char buffer[1024 * 1024] = {0};
	fread(buffer, sizeof(char), 1024 * 1024, f);
	fclose(f);
	return parse_expr(buffer);
}

void spit_function(char *path, solid_object *func)
{
	solid_bytecode *inslist = ((solid_function *) func->data)->bcode;
	solid_bytecode cur;
	int pos;
	FILE *f = fopen(path, "w");
	for (pos = 0, cur = inslist[pos]; cur.ins != OP_END; cur = inslist[++pos]) {
		fprintf(f, "%d %d %d %x\n", cur.ins, cur.a, cur.b, (unsigned int) cur.meta);
	}
}

void solid_compile(solid_vm *vm)
{
	vm->regs[255] = parse_tree(parse_expr(get_str_value(pop_stack(vm))));
}

void solid_import(solid_vm *vm)
{
	char *input = get_str_value(pop_stack(vm));
	char *dot = strrchr(input, '.');
	char *extension;
	if (!dot || dot == input) {
		extension = "";
	}
	extension = dot + 1;
	if (strcmp(extension, "sol") == 0) {
		solid_object *func = parse_tree(parse_file(input));
		solid_call_func(vm, func);
	} else if (strcmp(extension, "so") == 0) {
		void *handle = dlopen(input, RTLD_LAZY);
		void (*init)(solid_vm *);
		if (handle == NULL) {
			log_err("Loading external library %s failed with error %s", input, dlerror());
		}
		dlerror();
		*(void **) (&init) = dlsym(handle, "solid_init");
		init(vm);
		//dlclose(handle);
	}
}

void solid_repl()
{
	size_t n = 256;
	char *buffer = (char *) malloc(sizeof(char) * n);
	solid_vm *vm = make_solid_vm();
	set_namespace(get_current_namespace(vm), solid_str("compile"), define_c_function(solid_compile));
	set_namespace(get_current_namespace(vm), solid_str("import"), define_c_function(solid_import));
	while (printf("%s", "solid> "), getline(&buffer, &n, stdin) != -1) {
		solid_object *curexpr = parse_tree(parse_expr(buffer));
		solid_call_func(vm, curexpr);
		push_stack(vm, vm->regs[255]);
		solid_print(vm);
	}
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		solid_object *mainfunc = parse_tree(parse_file(argv[1]));
		solid_vm *vm = make_solid_vm();
		set_namespace(get_current_namespace(vm), solid_str("compile"), define_c_function(solid_compile));
		set_namespace(get_current_namespace(vm), solid_str("import"), define_c_function(solid_import));
		solid_call_func(vm, mainfunc);
	} else {
		solid_repl();
	}

	return 0;
}
