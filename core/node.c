#include "node.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <cuttle/utils.h>
#include <cuttle/debug.h>

solid_node_val NULL_VALUE = {0};

solid_node_val solid_null_value()
{
	return NULL_VALUE;
}

solid_node_val solid_int_value(int i)
{
	solid_node_val r;
	r.ival = i;
	return r;
}

solid_node_val solid_double_value(double d)
{
	solid_node_val r;
	r.dval = d;
	return r;
}

solid_node_val solid_str_value(char *s)
{
	solid_node_val r;
	if (s != NULL) {
		memset((void *) r.strval, 0x0, 256);
		memcpy(r.strval, s, strlen(s));
	}
	return r;
}

solid_ast_node *solid_make_node(solid_node_ins ins, solid_ast_node *arg1, solid_ast_node *arg2, solid_node_val val)
{
	solid_ast_node *r;
	if ((r = (solid_ast_node *) malloc(sizeof(solid_ast_node)))) {
		r->ins = ins;
		r->arg1 = arg1;
		r->arg2 = arg2;
		r->val = val;
		return r;
	}
	log_err("malloc failure in make_node");
	exit(1);
}

solid_ast_node *solid_const_integer_node(int i)
{
	return solid_make_node(CONST_INT, NULL, NULL, solid_int_value(i));
}

solid_ast_node *solid_const_double_node(double d)
{
	return solid_make_node(CONST_DOUBLE, NULL, NULL, solid_double_value(d));
}

solid_ast_node *solid_const_string_node(char *s)
{
	return solid_make_node(CONST_STR, NULL, NULL, solid_str_value(s));
}

solid_ast_node *solid_const_bool_node(int i)
{
	return solid_make_node(CONST_BOOL, NULL, NULL, solid_int_value(i));
}

solid_ast_node *solid_identifier_node(char *name)
{
	return solid_make_node(IDENTIFIER, NULL, NULL, solid_str_value(name));
}
