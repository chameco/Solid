#include "node.h"

node_val NULL_VALUE = {0};

node_val null_value()
{
	return NULL_VALUE;
}

node_val int_value(int i)
{
	node_val r;
	r.ival = i;
	return r;
}

node_val str_value(char *s)
{
	node_val r;
	if (s != NULL) {
		memset((void *) r.strval, 0x0, 256);
		memcpy(r.strval, s, strlen(s));
	}
	return r;
}

ast_node *make_node(node_ins ins, ast_node *arg1, ast_node *arg2, node_val val)
{
	ast_node *r;
	if ((r = (ast_node *) malloc(sizeof(ast_node)))) {
		r->ins = ins;
		r->arg1 = arg1;
		r->arg2 = arg2;
		r->val = val;
		return r;
	}
	log_err("malloc failure in make_node");
}

ast_node *const_integer_node(int i)
{
	return make_node(CONST_INT, NULL, NULL, int_value(i));
}

ast_node *const_string_node(char *s)
{
	return make_node(CONST_STR, NULL, NULL, str_value(s));
}

ast_node *const_bool_node(int i)
{
	return make_node(CONST_BOOL, NULL, NULL, int_value(i));
}

ast_node *identifier_node(char *name)
{
	return make_node(IDENTIFIER, NULL, NULL, str_value(name));
}
