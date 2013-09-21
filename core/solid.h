#ifndef SOLID_SOLID_H
#define SOLID_SOLID_H
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <cuttle/utils.h>
#include <cuttle/debug.h>

#include "scanner_state.h"
#include "node.h"
#include "vm.h"
#include "ast.h"
#include "common.h"
#include "lexer.h"

int yyparse(ast_node **expression, yyscan_t scanner);

ast_node *parse_expr(char *expr);

ast_node *parse_file(char *path);
#endif
