#ifndef NODE_H
#define NODE_H
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "debug.h"

#include "utils.h"

typedef struct ast_node {
	int x;
} ast_node;

typedef union YYSTYPE {
	ast_node *node;
} YYSTYPE;

#endif
