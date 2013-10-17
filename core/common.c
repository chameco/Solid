#include "common.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/* Print the formatted string from applying "..."
 *  to "fmt" and call perror if errno is non-zero. */
void report_error(char *fmt, ...)
{
    char *prefix = "Solid";
    char *error = NULL;
    int len = 0;
    va_list args;

    va_start(args, fmt);

    /* snprintf() returns the length of the
     *  string if the first argument is NULL. */
    len = vsnprintf(NULL, 0, fmt, args);

    error = calloc(1, len + 1);
    vsnprintf(error, len + 1, fmt, args);

    if(errno) {
        fprintf(stderr, "%s: ", prefix);
        perror(error);
    } else {
        fprintf(stderr, "%s: %s\n", prefix, error);
    }
    va_end(args);
    free(error);
}

int yyerror(struct YYLTYPE *yylloc_param, void *scanner, struct ast_node **root, const char *s)
{
    log_err("%s at line %d", s, yylloc_param->first_line);
    exit(1);
    return 0;
}
