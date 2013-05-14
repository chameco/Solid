#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "debug.h"

#include "node.h"
#include "vm.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

solid_bytecode i[256];

int yyparse(ast_node **expression, yyscan_t scanner);

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

	debug("value: %d", get_int_value(get_namespace(vm->global_ns, solid_str("var"))));
	debug("value: %d", get_int_value(get_namespace(vm->global_ns, solid_str("x"))));

	return 0;
}
