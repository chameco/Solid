#include "common.h"
#include <string.h>
#include <stdlib.h>

/* Print out a general error message in *string. Use perror if
 *	errno is set, otherwise just print out *string plus a prefix. */
void report_error(char *string)
{
	char *prefix = "Solid";
	char *error = NULL;
	int len = strlen(prefix) + strlen(string);

	error = calloc(1, len + 1);
	if(error == NULL) {
		error = prefix;
	} else {
		sprintf(error, "%s: %s", prefix, string);
	}

	if(errno) {
		perror(error);
	} else {
		fputs(error, stderr);
	}

	free(error);
}

int yyerror(struct YYLTYPE *yylloc_param, void *scanner, struct ast_node **root, const char *s)
{
	log_err("%s at line %d", s, yylloc_param->first_line);
	exit(1);
	return 0;
}
