#ifndef COMMON_H
#define COMMON_H
typedef void* yyscan_t;
typedef struct YYLTYPE  
{  
	int first_line;  
	int first_column;  
	int last_line;  
	int last_column;  
} YYLTYPE;
#define YYLTYPE_IS_DECLARED 1
int yylex (YYSTYPE *yyval_param, YYLTYPE *yylloc_param, yyscan_t yyscanner);
int yyerror(struct YYLTYPE *yylloc_param, yyscan_t scanner, ast_node **root, const char *s)
{
	log_err("%s at line %d", s, yylloc_param->first_line);
	exit(1);
	return 0;
}
#endif
