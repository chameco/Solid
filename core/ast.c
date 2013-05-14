#include "ast.h"

#define dbc(I, A, B, M) bcode[i++] = bc(I, A, B, M)
#define pn(N) i = parse_node(N, bcode, i)

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
	//debug("node->ins: %d", node->ins);
	switch (node->ins) {
		case STATEMENT_LIST:
			if (node->arg2 != NULL) {
				pn(node->arg2);
			}
			pn(node->arg1);
			break;
		case IDENTIFIER:
			log_err("ERROR: IDENTIFIER node shouldn't be parsed.");
			exit(1);
			break;
		case NS_VAR:
			log_err("ERROR: NS_VAR node shouldn't be parsed.");
			exit(1);
			break;
		case GET_VAR:
			if (node->arg1->arg2 != NULL) {
				pn(node->arg1->arg2);
				dbc(OP_POP, 1, 0, NULL);
			} else {
				dbc(OP_GLOBALNS, 1, 0, NULL);
			}
			dbc(OP_PUSHSTR, 0, 0, node->arg1->arg1->val.strval);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_GET, 1, 2, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case SET_VAR:
			pn(node->arg1);
			if (node->arg2->arg2 != NULL) {
				pn(node->arg2->arg2);
				dbc(OP_POP, 1, 0, NULL);
			} else {
				dbc(OP_GLOBALNS, 1, 0, NULL);
			}
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_SET, 1, 2, node->arg2->arg1->val.strval);
			dbc(OP_PUSHSTR, 0, 0, node->arg2->arg1->val.strval);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_GET, 1, 2, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CALL:
			break;
		case CONST_INT:
			dbc(OP_PUSHINT, node->val.ival, 0, NULL);
			break;
		case CONST_STR:
			dbc(OP_PUSHSTR, 0, 0, node->val.strval);
			break;
		case FUNC_ARGS:
			break;
		case PLUS:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_ADD, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case MINUS:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_SUB, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case MUL:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_MUL, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case DIV:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_DIV, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CEQ:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_EQ, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CLT:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_LT, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CLTE:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_LTE, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CGT:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_GT, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
		case CGTE:
			pn(node->arg1);
			pn(node->arg2);
			dbc(OP_POP, 2, 0, NULL);
			dbc(OP_POP, 3, 0, NULL);
			dbc(OP_GTE, 2, 3, NULL);
			dbc(OP_PUSH, 2, 0, NULL);
			break;
	}
	return i;
}
