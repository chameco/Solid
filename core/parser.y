%{
#define YYERROR_VERBOSE 1
#include "node.h"
#include <stddef.h>
%}

%output "parser.c"
%defines "parser.h"

%locations
%define api.pure
%lex-param {void *scanner}
%parse-param {solid_ast_node **root}
%parse-param {void *scanner}

%token <node> TIDENTIFIER TINLINE_IDENTIFIER TSTRING TINTEGER TDOUBLE
%token <token> TTRUE TFALSE

%token <token> TSEMICOLON;
%token <token> TLPAREN TRPAREN TLSQUARE TRSQUARE TLBRACE TRBRACE TAT TDOLLAR TCOMMA TDOT TTILDE

%token <token> TIF TWHILE TEQUALS TGLOBAL TFN TNS TRETURN

%type <node> program

%type <node> stmt_list ns_var expr constant identifier func_args param_list list_items

%start program

%%

program : stmt_list {*root = $1;}
	;

stmt_list : expr TSEMICOLON {$$ = solid_make_node(STATEMENT_LIST, $1, NULL, solid_null_value());}
	  | expr TSEMICOLON stmt_list {$$ = solid_make_node(STATEMENT_LIST, $1, $3, solid_null_value());}
	  | expr error TSEMICOLON stmt_list {$$ = solid_make_node(STATEMENT_LIST, $1, $4, solid_null_value());}
	  ;

ns_var : identifier {$$ = solid_make_node(NS_VAR, $1, NULL, solid_null_value());}
       | TDOLLAR {$$ = solid_make_node(NS_VAR, solid_identifier_node("$"), NULL, solid_null_value());}
       | expr TDOT identifier {$$ = solid_make_node(NS_VAR, $3, $1, solid_null_value());}
       ;


expr : constant {$$ = $1;}
     | TAT identifier {$$ = solid_make_node(GGET, $2, NULL, solid_null_value());}
     | expr TINLINE_IDENTIFIER expr {$$ = solid_make_node(CALL,
     solid_make_node(GET, solid_make_node(NS_VAR, $2, NULL, solid_null_value()), NULL, solid_null_value()),
     solid_make_node(FUNC_ARGS, $3, solid_make_node(FUNC_ARGS, $1, NULL, solid_null_value()), solid_null_value()), solid_null_value());}
     | expr TLPAREN func_args TRPAREN {$$ = solid_make_node(CALL, $1, $3, solid_null_value());}
     | ns_var {$$ = solid_make_node(GET, $1, NULL, solid_null_value());}
     | ns_var TEQUALS expr {$$ = solid_make_node(SET, $3, $1, solid_null_value());}
     | identifier TGLOBAL expr {$$ = solid_make_node(GSET, $3, $1, solid_null_value());}
     | TLBRACE stmt_list TRBRACE {$$ = solid_make_node(BLOCK, $2, NULL, solid_null_value());}
     | TIF expr expr {$$ = solid_make_node(IF, $2, $3, solid_null_value());}
     | TWHILE expr expr {$$ = solid_make_node(WHILE, $2, $3, solid_null_value());}
     | TFN param_list expr {$$ = solid_make_node(FN, $2, $3, solid_null_value());}
     | TNS expr {$$ = solid_make_node(NS, $2, NULL, solid_null_value());}
     | TRETURN expr {$$ = solid_make_node(RET, $2, NULL, solid_null_value());}
     ;

constant : TSTRING {$$ = $1;}
	 | TINTEGER {$$ = $1;}
	 | TDOUBLE {$$ = $1;}
	 | TTRUE {$$ = solid_const_bool_node(1);}
	 | TFALSE {$$ = solid_const_bool_node(0);}
	 | TLSQUARE list_items TRSQUARE {$$ = solid_make_node(CONST_LIST, $2, NULL, solid_null_value());}
	 ;

identifier : TIDENTIFIER | TINLINE_IDENTIFIER {$$ = $1;}
	   ;

param_list : /* blank */ {$$ = solid_make_node(PARAM_LIST, NULL, NULL, solid_null_value());}
	   | TTILDE {$$ = solid_make_node(PARAM_LIST, NULL, NULL, solid_null_value());}
	   | TIDENTIFIER {$$ = solid_make_node(PARAM_LIST, $1, NULL, solid_null_value());}
	   | param_list TCOMMA TIDENTIFIER {$$ = solid_make_node(PARAM_LIST, $3, $1, solid_null_value());}
	   ;

func_args : /* blank */ {$$ = solid_make_node(FUNC_ARGS, NULL, NULL, solid_null_value());}
	  | expr {$$ = solid_make_node(FUNC_ARGS, $1, NULL, solid_null_value());}
	  | func_args TCOMMA expr {$$ = solid_make_node(FUNC_ARGS, $3, $1, solid_null_value());}
	  ;

list_items : /* blank */ {$$ = solid_make_node(LIST_BODY, NULL, NULL, solid_null_value());}
	   | expr {$$ = solid_make_node(LIST_BODY, $1, NULL, solid_null_value());}
	   | list_items TCOMMA expr {$$ = solid_make_node(LIST_BODY, $3, $1, solid_null_value());}
	   ;

%%
