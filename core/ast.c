#include "ast.h"

#define dbc(I, A, B, M) bcode[i++] = bc(I, A, B, M)
#define pn(N) i = parse_node(N, bcode, i)
#define fdbc(I, A, B, M) function_bcode[fi++] = bc(I, A, B, M)
#define fpn(N) fi = parse_node(N, function_bcode, fi)

solid_object *parse_tree(ast_node *tree)
{
	solid_bytecode *bcode = (solid_bytecode *) malloc(
			sizeof(solid_bytecode) * 1024);
	int i = parse_node(tree, bcode, 0);
	dbc(OP_END, 0, 0, NULL);
	return define_function(bcode);
}
int parse_node(ast_node *node, solid_bytecode *bcode, int i)
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
				dbc(OP_POP, 1, 0, NULL);
			} else {
				dbc(OP_LOCALNS, 1, 0, NULL);
			}
			dbc(OP_PUSHSTR, 0, 0, node->arg1->arg1->val.strval);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_GET, 1, 2, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case DEFINE:
			pn(node->arg1);
			if (node->arg2->arg2 != NULL) {
				pn(node->arg2->arg2);
				dbc(OP_POP, 1, 0, NULL);
			} else {
				dbc(OP_LOCALNS, 1, 0, NULL);
			}
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_SET, 1, 2, node->arg2->arg1->val.strval);
			dbc(OP_PUSHSTR, 0, 0, node->arg2->arg1->val.strval);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_GET, 1, 2, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CALL:
			pn(node->arg2);
			pn(node->arg1); //TODO: Args support
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_CALL, 2, 0, NULL);
			break;
		case FUNC_ARGS:
			break;
		case CONST_INT:
			dbc(OP_PUSHINT, node->val.ival, 0, NULL);
			break;
		case CONST_STR:
			dbc(OP_PUSHSTR, 0, 0, node->val.strval);
			break;
		case CONST_BOOL:
			dbc(OP_PUSHBOOL, node->val.ival, 0, NULL);
			break;
		case IF:
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_NOT, 2, 0, NULL);
			temp[0] = i;
			dbc(OP_NOP, 0, 0, NULL);
			pn(node->arg2);
			bcode[temp[0]] = bc(OP_JMPIF, i, 2, NULL);
			break;
		case WHILE:
			temp[0] = i;
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_NOT, 2, 0, NULL);
			temp[1] = i;
			dbc(OP_NOP, 0, 0, NULL);
			pn(node->arg2);
			dbc(OP_JMP, temp[0] - 1, 0, NULL);
			bcode[temp[1]] = bc(OP_JMPIF, i, 2, NULL);
			break;
		case FN:
			function_bcode = (solid_bytecode *) malloc(
					sizeof(solid_bytecode) * 1024);
			fi = 0;
			fpn(node->arg1);
			fpn(node->arg2);
			fdbc(OP_RETURN, 0, 0, NULL);
			dbc(OP_FN, 0, 0, function_bcode);
			function_bcode = NULL;
			fi = 0;
			break;
		case PARAM_LIST:
			break; //TODO: Actual arguments
		case RETURN:
			pn(node->arg1);
			dbc(OP_RETURN, 0, 0, NULL);
			break;
		case CLASS:
			if (node->arg1 != NULL) {
				pn(node->arg1);
			} else {
				dbc(OP_PUSHINT, 0, 0, NULL);
			}
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_CLASS, 2, 0, NULL);
			dbc(OP_POP, 0, 0, NULL);
			pn(node->arg2);
			dbc(OP_ENDCLASS, 0, 0, NULL);
			dbc(OP_PUSH, 0, 0, NULL);
			break;
		case NEW:
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_NEW, 2, 0, NULL);
			break;
		case PLUS:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_ADD, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case MINUS:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_SUB, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case MUL:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_MUL, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case DIV:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_DIV, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CEQ:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_EQ, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CLT:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_LT, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CLTE:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_LTE, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CGT:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_GT, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CGTE:
			pn(node->arg2);
			pn(node->arg1);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_GTE, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
	}
	return i;
}
