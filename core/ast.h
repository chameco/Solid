#ifndef AST_H
#define AST_H
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "debug.h"

#include "utils.h"
#include "node.h"
#include "vm.h"
#include "object.h"

solid_object *parse_tree(ast_node *tree);
int parse_node(ast_node *node, solid_bytecode *bcode, int i);

#endif
