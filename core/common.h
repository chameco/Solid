#ifndef SOLID_COMMON_H
#define SOLID_COMMON_H
#include <stdlib.h>
#include <cuttle/debug.h>
typedef struct YYLTYPE  
{  
	int first_line;  
	int first_column;  
	int last_line;  
	int last_column;  
} YYLTYPE;
#define YYLTYPE_IS_DECLARED 1
struct ast_node;
int yyerror(struct YYLTYPE *yylloc_param, void *scanner, struct ast_node **root, const char *s);
#endif
