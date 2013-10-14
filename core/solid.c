#include "solid.h"

int yyparse(solid_ast_node **expression, yyscan_t scanner);

solid_ast_node *solid_parse_expr(char *expr)
{
	solid_ast_node *ret = NULL;
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

solid_ast_node *solid_parse_file(char *path)
{
	FILE *f = fopen(path, "r");

	if(f == NULL) {
		return NULL;
	}

	char buffer[1024 * 1024] = {0};
	fread(buffer, sizeof(char), 1024 * 1024, f);
	fclose(f);
	return solid_parse_expr(buffer);
}

void solid_compile(solid_vm *vm)
{
	vm->regs[255] = solid_parse_tree(solid_parse_expr(solid_get_str_value(solid_pop_stack(vm))));
}

void solid_import(solid_vm *vm)
{
	char *input = solid_get_str_value(solid_pop_stack(vm));
	char *dot = strrchr(input, '.');
	char *extension;
	if (!dot || dot == input) {
		extension = "";
	}
	extension = dot + 1;
	if (strcmp(extension, "sol") == 0) {
		solid_object *func = solid_parse_tree(solid_parse_file(input));
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
	solid_vm *vm = solid_make_vm();
	solid_set_namespace(solid_get_current_namespace(vm), solid_str("compile"), solid_define_c_function(solid_compile));
	solid_set_namespace(solid_get_current_namespace(vm), solid_str("import"), solid_define_c_function(solid_import));
	while (printf("%s", "solid> "), getline(&buffer, &n, stdin) != -1) {
		solid_object *curexpr = solid_parse_tree(solid_parse_expr(buffer));
		solid_call_func(vm, curexpr);
		solid_push_stack(vm, vm->regs[255]);
		solid_print(vm);
	}
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		solid_ast_node *tree = solid_parse_file(argv[1]);
		if(tree == NULL) {
			report_error(argv[1]);
			exit(EXIT_FAILURE);
		}

		solid_object *mainfunc = solid_parse_tree(tree);
		solid_vm *vm = solid_make_vm();
		solid_set_namespace(solid_get_current_namespace(vm), solid_str("compile"), solid_define_c_function(solid_compile));
		solid_set_namespace(solid_get_current_namespace(vm), solid_str("import"), solid_define_c_function(solid_import));
		solid_call_func(vm, mainfunc);

		/* [FIXME: Track down all the other 'allocs that need to be free'd] */
		free(vm);
	} else {
		solid_repl();
	}

	return 0;
}
