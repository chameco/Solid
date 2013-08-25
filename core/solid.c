#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "debug.h"

#include "node.h"
#include "vm.h"
#include "ast.h"
#include "common.h"
#include "lexer.h"

solid_bytecode i[256];

int yyparse(ast_node **expression, yyscan_t scanner);

void d_type(solid_vm *vm, char *name)
{
	debug("%s: type of %d", name, get_namespace(vm->namespace_stack[0], solid_str(name))->type);
}

void d_int(solid_vm *vm, char *name)
{
	debug("%s: %d, type of %d", name, get_int_value(get_namespace(vm->namespace_stack[0], solid_str(name))), get_namespace(vm->namespace_stack[0], solid_str(name))->type);
}

ast_node *parse_expr(char *expr)
{
	ast_node *ret = NULL;
	yyscan_t scanner;
	YY_BUFFER_STATE state;

	yylex_init(&scanner);
	state = yy_scan_string(expr, scanner);
	yyparse(&ret, scanner);
	yy_delete_buffer(state, scanner);
	yylex_destroy(scanner);

	return ret;
}

ast_node *parse_file(char *path)
{
	FILE *f = fopen(path, "r");
	char buffer[1024 * 1024];
	fread(buffer, sizeof(char), 1024 * 1024, f);
	fclose(f);
	return parse_expr(buffer);
}

int main(int argc, char *argv[])
{
	solid_object *testfunc = parse_tree(parse_file(argv[1]));

	solid_vm *vm = make_solid_vm();

	solid_eval_bytecode(vm, testfunc);

	d_int(vm, "var");
	d_int(vm, "x");
	//debug("c: %d", get_int_value(get_namespace(vm->namespace_stack[0], solid_str("c"))));
	//debug("testfunc: %d", get_int_value(get_namespace(vm->namespace_stack[0], solid_str("testfunc"))));
	//debug("testclass.c: %d", get_int_value(get_namespace(get_namespace(vm->namespace_stack[0], solid_str("testclass")), solid_str("c"))));
	//debug("class2.c: %d", get_int_value(get_namespace(get_namespace(vm->namespace_stack[0], solid_str("class2")), solid_str("c"))));
	//debug("z: %d", get_int_value(get_namespace(vm->namespace_stack[0], solid_str("z"))));

	return 0;
}
