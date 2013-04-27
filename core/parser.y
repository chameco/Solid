%{
#include "node.h"
#include "lexer.h"
	typedef void* yyscan_t;
	int yyerror(yyscan_t scanner, ast_node *root, const char *s);
%}

%output "parser.c"
%defines "parser.h"

%define api.pure
%lex-param {yyscan_t scanner}
%parse-param {ast_node *root}
%parse-param {yyscan_t scanner}

%token TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token TNEWLINE;
%token TLPAREN TRPAREN TLSQUARE TRSQUARE TLBRACE TRBRACE TCOMMA TDOT
%token TPLUS TMINUS TMUL TDIV

%type <node> program

%start program

%%

program : TSEMICOLON {root = NULL;}
	;

%%
