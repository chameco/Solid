%{
#include "node.h"
#include "lexer.h"
	typedef void* yyscan_t;
	int yyerror(yyscan_t scanner, ast_node **root, const char *s);
%}

%output "parser.c"
%defines "parser.h"

%define api.pure
%lex-param {yyscan_t scanner}
%parse-param {ast_node **root}
%parse-param {yyscan_t scanner}

%token <node> TIDENTIFIER TSTRING TINTEGER
%token <token> TCEQ TCLT TCLE TCGT TCGE TEQUAL
%token <token> TSEMICOLON;
%token <token> TLPAREN TRPAREN TLSQUARE TRSQUARE TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV

%type <node> program

%type <node> stmt_list ns_var expr constant identifier func_args

%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : stmt_list {*root = $1;}
	;

stmt_list : expr TSEMICOLON {$$ = make_node(STATEMENT_LIST, $1, NULL, null_value());}
	  | stmt_list expr TSEMICOLON {$$ = make_node(STATEMENT_LIST, $2, $1, null_value());}
	  ;

ns_var : identifier {$$ = make_node(NS_VAR, $1, NULL, null_value());}
       | expr TDOT identifier {$$ = make_node(NS_VAR, $3, $1, null_value());}
       ;


expr : constant {$$ = $1;}
     | ns_var {$$ = make_node(GET_VAR, $1, NULL, null_value());}
     | ns_var TEQUAL expr {$$ = make_node(SET_VAR, $3, $1, null_value());}
     | expr TLPAREN func_args TRPAREN {$$ = make_node(CALL, $1, $3, null_value());}
     | expr TPLUS expr {$$ = make_node(PLUS, $1, $3, null_value());}
     | expr TMINUS expr {$$ = make_node(MINUS, $1, $3, null_value());}
     | expr TMUL expr {$$ = make_node(MUL, $1, $3, null_value());}
     | expr TDIV expr {$$ = make_node(DIV, $1, $3, null_value());}
     | expr TCEQ expr {$$ = make_node(CEQ, $1, $3, null_value());}
     | expr TCLT expr {$$ = make_node(CLT, $1, $3, null_value());}
     | expr TCLE expr {$$ = make_node(CLTE, $1, $3, null_value());}
     | expr TCGT expr {$$ = make_node(CGT, $1, $3, null_value());}
     | expr TCGE expr {$$ = make_node(CGTE, $1, $3, null_value());}
     ;

constant : TSTRING {$$ = $1;}
	 | TINTEGER {$$ = $1;}
	 ;

identifier : TIDENTIFIER {$$ = $1;}
	   ;

func_args : expr {$$ = make_node(FUNC_ARGS, $1, NULL, null_value());}
	  | func_args TCOMMA expr {$$ = make_node(FUNC_ARGS, $3, $1, null_value());}
	  ;

%%
