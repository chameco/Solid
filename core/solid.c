#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "debug.h"

#include "node.h"
#include "vm.h"
#include "lexer.h"
#include "parser.h"

solid_bytecode i[256];

int yyparse(ast_node *expression, yyscan_t scanner);

ast_node *parse_expr(char *expr)
{
	ast_node *ret = NULL;
	yyscan_t scanner;
	YY_BUFFER_STATE state;

	yylex_init(&scanner);
	state = yy_scan_string(expr, scanner);
	yyparse(ret, scanner);
	yy_delete_buffer(state, scanner);
	yylex_destroy(scanner);

	return ret;
}

solid_bytecode bc(solid_ins i, int a, int b, char *meta)
{
	solid_bytecode ret;
	ret.ins = i;
	ret.a = a;
	ret.b = b;
	ret.meta = meta;
	return ret;
}

int main(int argc, char *argv[])
{
	// PARSER TEST //
	parse_expr(";");
	/////////////////
	// VM TEST //
	i[0] = bc(OP_PUSHINT, 1337, 0, NULL);
	i[1] = bc(OP_POP, 1, 0, NULL);
	i[2] = bc(OP_GLOBALNS, 2, 0, NULL);
	i[3] = bc(OP_SET, 2, 1, "var");
	i[4] = bc(OP_END, 0, 0, NULL);
	solid_vm *vm = make_solid_vm();
	solid_object *testfunc = define_function(i);
	solid_eval_bytecode(vm, testfunc);
	debug("value: %d", get_int_value(get_namespace(vm->global_ns, solid_str("var"))));
	/////////////

	return 0;
}
