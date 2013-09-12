#ifndef SOLID_NODE_H
#define SOLID_NODE_H
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <cuttle/utils.h>
#include <cuttle/debug.h>

typedef enum node_ins {
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
	IF,
	WHILE,
	FN,
	PARAM_LIST,
	RETURN,
	CLASS,
	NEW,
	PLUS,
	MINUS,
	MUL,
	DIV,
	CEQ,
	CLT,
	CLTE,
	CGT,
	CGTE,
} node_ins;

typedef union node_val {
	int ival;
	char strval[256];
} node_val;

typedef struct ast_node {
	node_ins ins;
	struct ast_node *arg1;
	struct ast_node *arg2;
	node_val val;
} ast_node;

typedef union YYSTYPE {
	int token;
	ast_node *node;
} YYSTYPE;

node_val null_value();
node_val int_value(int i);
node_val str_value(char *s);

ast_node *make_node(node_ins ins, ast_node *arg1, ast_node *arg2, node_val val);

ast_node *const_integer_node(int i);
ast_node *const_string_node(char *s);
ast_node *const_bool_node(int i);
ast_node *identifier_node(char *name);

#endif
