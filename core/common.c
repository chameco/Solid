#include "common.h"

int yyerror(struct YYLTYPE *yylloc_param, void *scanner, struct ast_node **root, const char *s)
{
	log_err("%s at line %d", s, yylloc_param->first_line);
	exit(1);
	return 0;
}
