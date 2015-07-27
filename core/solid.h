#ifndef SOLID_SOLID_H
#define SOLID_SOLID_H
#define _GNU_SOURCE
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#ifndef WINDOWS
#include <dlfcn.h>
#endif
#include <unistd.h>

#include "utils.h"
#include "scanner_state.h"
#include "node.h"
#include "vm.h"
#include "ast.h"
#include "common.h"
#include "lexer.h"

int yyparse(solid_ast_node **expression, yyscan_t scanner);

solid_ast_node *solid_parse_expr(char *expr);

solid_ast_node *solid_parse_file(char *path);

void solid_compile(solid_vm *vm);

void solid_import(solid_vm *vm);
#endif
