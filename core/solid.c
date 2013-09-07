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

void solid_print(solid_vm *vm)
{
	solid_object *in = pop_stack(vm);
	char *input = get_str_value(in);
	fprintf(stdout, "%s\n", input);
	vm->regs[255] = in;
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

int main(int argc, char *argv[])
{
	solid_object *testfunc = parse_tree(parse_file(argv[1]));

	spit_function("test.list", testfunc);

	solid_vm *vm = make_solid_vm();

	set_namespace(get_current_namespace(vm), solid_str("print"), define_c_function(solid_print));

	solid_call_func(vm, testfunc);

	d_int(vm, "y");

	return 0;
}
