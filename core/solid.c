#include "solid.h"
#ifndef WINDOWS
#include "linenoise/linenoise.h"
#endif

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
	vm->regs[255] = solid_parse_tree(vm, solid_parse_expr(solid_get_str_value(solid_pop_stack(vm))));
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
		solid_object *func = solid_parse_tree(vm, solid_parse_file(input));
		solid_call_func(vm, func);
	} else if (strcmp(extension, "so") == 0) {
#ifndef WINDOWS
		char buffer[256];
		getcwd(buffer, 256);
		strcat(buffer, "/");
		strcat(buffer, input);
		void *handle = dlopen(buffer, RTLD_LAZY);
		void (*init)(solid_vm *);
		if (handle == NULL) {
			log_err("Loading external library %s failed with error %s", input, dlerror());
			exit(1);
		}
		dlerror();
		*(void **) (&init) = dlsym(handle, "solid_init");
		init(vm);
		//dlclose(handle);
#endif
	}
}

#ifndef WINDOWS
void solid_repl()
{
	char *input;
	char *buffer;
	solid_vm *vm = solid_make_vm();
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "compile"), solid_define_c_function(vm, solid_compile));
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "import"), solid_define_c_function(vm, solid_import));
	while ((input = linenoise("solid> ")) != NULL) {
		if (input[0] != '\0') {
			buffer = calloc(strlen(input) + 1, sizeof(char));
			strcpy(buffer, input);
			buffer[strlen(input)] = '\n';
			linenoiseHistoryAdd(input);
			solid_object *curexpr = solid_parse_tree(vm, solid_parse_expr(buffer));
			//free(buffer);
			solid_call_func(vm, curexpr);
			solid_push_stack(vm, vm->regs[255]);
			solid_print(vm);
		}
		//free(input);
	}
}
#endif

int main(int argc, char *argv[])
{
	if (argc > 1) {
		solid_ast_node *tree = solid_parse_file(argv[1]);
		if(tree == NULL) {
			report_error(argv[1]);
			exit(EXIT_FAILURE);
		}

		solid_vm *vm = solid_make_vm();
		solid_object *mainfunc = solid_parse_tree(vm, tree);
		solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "compile"), solid_define_c_function(vm, solid_compile));
		solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "import"), solid_define_c_function(vm, solid_import));
		solid_call_func(vm, mainfunc);

		/* [FIXME: Track down all the other 'allocs that need to be free'd] */
		//free(vm);
	}
#ifndef WINDOWS
	else {
		solid_repl();
	}
#endif

	return 0;
}
