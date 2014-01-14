#ifndef SOLID_AST_H
#define SOLID_AST_H

#include "node.h"
#include "vm.h"
#include "object.h"

solid_object *solid_parse_tree(solid_vm *vm, solid_ast_node *tree);
int solid_parse_node(solid_ast_node *node, solid_bytecode *bcode, int i);

#endif
