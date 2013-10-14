#ifndef SOLID_NODE_H
#define SOLID_NODE_H
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <cuttle/utils.h>
#include <cuttle/debug.h>

typedef enum solid_node_ins {
	STATEMENT_LIST,
	BLOCK,
	IDENTIFIER,
	NS_VAR,
	GET,
	SET,
	CALL,
	FUNC_ARGS,
	CONST_INT,
	CONST_STR,
	CONST_BOOL,
	CONST_LIST,
	LIST_BODY,
	IF,
	WHILE,
	FN,
	PARAM_LIST,
	RET,
	NS
} solid_node_ins;

typedef union solid_node_val {
	int ival;
	char strval[256];
} solid_node_val;

typedef struct solid_ast_node {
	solid_node_ins ins;
	struct solid_ast_node *arg1;
	struct solid_ast_node *arg2;
	solid_node_val val;
} solid_ast_node;

typedef union YYSTYPE {
	int token;
	solid_ast_node *node;
} YYSTYPE;

solid_node_val solid_null_value();
solid_node_val solid_int_value(int i);
solid_node_val solid_str_value(char *s);

solid_ast_node *solid_make_node(solid_node_ins ins, solid_ast_node *arg1, solid_ast_node *arg2, solid_node_val val);

solid_ast_node *solid_const_integer_node(int i);
solid_ast_node *solid_const_string_node(char *s);
solid_ast_node *solid_const_bool_node(int i);
solid_ast_node *solid_identifier_node(char *name);

#endif
