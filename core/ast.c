#include "ast.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "utils.h"
#include "node.h"
#include "vm.h"
#include "object.h"

#define dbc(I, A, B, M) bcode[i++] = solid_bc(I, A, B, M)
#define pn(N) i = solid_parse_node(N, bcode, i)
#define fdbc(I, A, B, M) function_bcode[fi++] = solid_bc(I, A, B, M)
#define fpn(N) fi = solid_parse_node(N, function_bcode, fi)

solid_object *solid_parse_tree(solid_vm *vm, solid_ast_node *tree)
{
	solid_bytecode *bcode = (solid_bytecode *) malloc(
			sizeof(solid_bytecode) * 1024);
	int i = solid_parse_node(tree, bcode, 0);
	dbc(OP_END, 0, 0, NULL);
	return solid_define_function(vm, bcode);
}
int solid_parse_node(solid_ast_node *node, solid_bytecode *bcode, int i)
{
	int temp[8] = {0};
	solid_bytecode *function_bcode = NULL;
	int fi = 0;
	//debug("parsing node with ins %d", node->ins);
	switch (node->ins) {
		case STATEMENT_LIST:
			pn(node->arg1);
			if (node->arg2 != NULL) {
				pn(node->arg2);
			}
			break;
		case BLOCK:
			pn(node->arg1);
			break;
		case IDENTIFIER:
			log_err("IDENTIFIER node shouldn't be parsed.");
			exit(1);
			break;
		case NS_VAR:
			log_err("NS_VAR node shouldn't be parsed.");
			exit(1);
			break;
		case GET:
			if (node->arg1->arg2 != NULL) {
				pn(node->arg1->arg2);
				dbc(OP_MOV, 1, 255, NULL);
			} else {
				dbc(OP_LOCALNS, 1, 0, NULL);
			}
			dbc(OP_STORESTR, 2, 0, node->arg1->arg1->val.strval);
			dbc(OP_GET, 2, 1, NULL);
			dbc(OP_MOV, 255, 2, NULL);
			break;
		case GGET:
			dbc(OP_GLOBALNS, 1, 0, NULL);
			dbc(OP_STORESTR, 2, 0, node->arg1->val.strval);
			dbc(OP_GET, 2, 1, NULL);
			dbc(OP_MOV, 255, 2, NULL);
			break;
		case SET:
			pn(node->arg1);
			dbc(OP_PUSH, 255, 0, NULL);
			if (node->arg2->arg2 != NULL) {
				pn(node->arg2->arg2);
				dbc(OP_MOV, 1, 255, NULL);
			} else {
				dbc(OP_LOCALNS, 1, 0, NULL);
			}
			dbc(OP_POP, 255, 0, NULL);
			dbc(OP_SET, 255, 1, node->arg2->arg1->val.strval);
			break;
		case GSET:
			pn(node->arg1);
			dbc(OP_GLOBALNS, 1, 0, NULL);
			dbc(OP_SET, 255, 1, node->arg2->val.strval);
			break;
		case CALL:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_CALL, 255, 0, NULL);
			break;
		case FUNC_ARGS:
			if (node->arg2 != NULL) {
				pn(node->arg2);
			}
			if (node->arg1 != NULL) {
				pn(node->arg1);
				dbc(OP_PUSH, 255, 0, NULL);
			}
			break;
		case CONST_INT:
			dbc(OP_STOREINT, 255, node->val.ival, NULL);
			break;
		case CONST_DOUBLE:
			dbc(OP_STOREDOUBLE, 255, 0, &(node->val.dval));
			break;
		case CONST_STR:
			dbc(OP_STORESTR, 255, 0, node->val.strval);
			break;
		case CONST_BOOL:
			dbc(OP_STOREBOOL, 255, node->val.ival, NULL);
			break;
		case CONST_LIST:
			dbc(OP_STORELIST, 0, 0, NULL);
			pn(node->arg1);
			break;
		case LIST_BODY:
			if (node->arg1 != NULL) {
				pn(node->arg1);
				dbc(OP_PUSHLIST, 0, 255, NULL);
			}
			if (node->arg2 != NULL) {
				pn(node->arg2);
			}
			dbc(OP_MOV, 255, 0, NULL);
			break;
		case IF:
			pn(node->arg1);
			dbc(OP_MOV, 2, 255, NULL);
			dbc(OP_NOT, 2, 0, NULL);
			temp[0] = i;
			dbc(OP_NOP, 0, 0, NULL);
			pn(node->arg2);
			bcode[temp[0]] = solid_bc(OP_JMPIF, i, 2, NULL);
			break;
		case WHILE:
			temp[0] = i;
			pn(node->arg1);
			dbc(OP_MOV, 2, 255, NULL);
			dbc(OP_NOT, 2, 0, NULL);
			temp[1] = i;
			dbc(OP_NOP, 0, 0, NULL);
			pn(node->arg2);
			dbc(OP_JMP, temp[0], 0, NULL);
			bcode[temp[1]] = solid_bc(OP_JMPIF, i, 2, NULL);
			break;
		case FN:
			function_bcode = (solid_bytecode *) malloc(
					sizeof(solid_bytecode) * 1024);
			fi = 0;
			if (node->arg1 != NULL) {
				fpn(node->arg1);
			}
			fpn(node->arg2);
			dbc(OP_FN, 255, 0, function_bcode);
			function_bcode = NULL;
			fi = 0;
			break;
		case PARAM_LIST:
			if (node->arg1 != NULL) {
				dbc(OP_LOCALNS, 1, 0, NULL);
				dbc(OP_POP, 2, 0, NULL);
				dbc(OP_SET, 2, 1, node->arg1->val.strval);
			}
			if (node->arg2 != NULL) {
				pn(node->arg2);
			}
			break;
		case RET:
			pn(node->arg1);
			dbc(OP_END, 0, 0, NULL);
			break;
		case NS:
			dbc(OP_NS, 255, 0, NULL);
			pn(node->arg1);
			dbc(OP_ENDNS, 255, 0, NULL);
	}
	return i;
}
