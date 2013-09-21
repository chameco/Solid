%{
#define YYERROR_VERBOSE 1
#include "node.h"
%}

%output "parser.c"
%defines "parser.h"

%locations
%define api.pure
%lex-param {void *scanner}
%parse-param {ast_node **root}
%parse-param {void *scanner}

%token <node> TIDENTIFIER TINLINE_IDENTIFIER TSTRING TINTEGER
%token <token> TTRUE TFALSE

%token <token> TSEMICOLON TNEWLINE;
%token <token> TLPAREN TRPAREN TLSQUARE TRSQUARE TLBRACE TRBRACE TCOMMA TDOT

%token <token> TIF TWHILE TEQUALS TFN TNS TRETURN

%type <node> program

%type <node> stmt_list ns_var expr constant identifier func_args param_list list_items

%start program

%%

program : stmt_list {*root = $1;}
	;

stmt_list : expr TSEMICOLON {$$ = make_node(STATEMENT_LIST, $1, NULL, null_value());}
	  | expr TSEMICOLON stmt_list {$$ = make_node(STATEMENT_LIST, $1, $3, null_value());}
	  | expr error TSEMICOLON stmt_list {$$ = make_node(STATEMENT_LIST, $1, $4, null_value());}
	  ;

ns_var : identifier {$$ = make_node(NS_VAR, $1, NULL, null_value());}
       | expr TDOT identifier {$$ = make_node(NS_VAR, $3, $1, null_value());}
       ;


expr : constant {$$ = $1;}
     | expr TINLINE_IDENTIFIER expr {$$ = make_node(CALL,
     make_node(GET, make_node(NS_VAR, $2, NULL, null_value()), NULL, null_value()),
     make_node(FUNC_ARGS, $3, make_node(FUNC_ARGS, $1, NULL, null_value()), null_value()), null_value());}
     | expr TLPAREN func_args TRPAREN {$$ = make_node(CALL, $1, $3, null_value());}
     | ns_var {$$ = make_node(GET, $1, NULL, null_value());}
     | ns_var TEQUALS expr {$$ = make_node(SET, $3, $1, null_value());}
     | TLBRACE stmt_list TRBRACE {$$ = make_node(BLOCK, $2, NULL, null_value());}
     | TIF expr expr {$$ = make_node(IF, $2, $3, null_value());}
     | TWHILE expr expr {$$ = make_node(WHILE, $2, $3, null_value());}
     | TFN param_list expr {$$ = make_node(FN, $2, $3, null_value());}
     | TNS expr {$$ = make_node(NS, $2, NULL, null_value());}
     | TRETURN expr {$$ = make_node(RETURN, $2, NULL, null_value());}
     ;

constant : TSTRING {$$ = $1;}
	 | TINTEGER {$$ = $1;}
	 | TTRUE {$$ = const_bool_node(1);}
	 | TFALSE {$$ = const_bool_node(0);}
	 | TLSQUARE list_items TRSQUARE {$$ = make_node(CONST_LIST, $2, NULL, null_value());}
	 ;

identifier : TIDENTIFIER | TINLINE_IDENTIFIER {$$ = $1;}
	   ;

param_list : /* blank */ {$$ = make_node(PARAM_LIST, NULL, NULL, null_value());}
	   | TIDENTIFIER {$$ = make_node(PARAM_LIST, $1, NULL, null_value());}
	   | param_list TCOMMA TIDENTIFIER {$$ = make_node(PARAM_LIST, $3, $1, null_value());}
	   ;

func_args : /* blank */ {$$ = make_node(FUNC_ARGS, NULL, NULL, null_value());}
	  | expr {$$ = make_node(FUNC_ARGS, $1, NULL, null_value());}
	  | func_args TCOMMA expr {$$ = make_node(FUNC_ARGS, $3, $1, null_value());}
	  ;

list_items : /* blank */ {$$ = make_node(LIST_BODY, NULL, NULL, null_value());}
	   | expr {$$ = make_node(LIST_BODY, $1, NULL, null_value());}
	   | list_items TCOMMA expr {$$ = make_node(LIST_BODY, $3, $1, null_value());}
	   ;

%%
