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

%token <node> TIDENTIFIER TSTRING TINTEGER
%token <token> TTRUE TFALSE

%token <token> TCEQ TCLT TCLE TCGT TCGE

%token <token> TSEMICOLON;
%token <token> TLPAREN TRPAREN TLSQUARE TRSQUARE TLBRACE TRBRACE TCOMMA TDOT

%token <token> TIF TWHILE TDEF TFN TRETURN TCLASS TNEW

%type <node> program

%type <node> stmt_list ns_var expr constant identifier func_args param_list

%left TPLUS TMINUS
%left TMUL TDIV

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
     | expr TLPAREN func_args TRPAREN {$$ = make_node(CALL, $1, $3, null_value());}
     | ns_var {$$ = make_node(GET, $1, NULL, null_value());}
     | TDEF ns_var expr {$$ = make_node(DEFINE, $3, $2, null_value());}
     | TNEW expr {$$ = make_node(NEW, $2, NULL, null_value());}
     | expr TPLUS expr {$$ = make_node(PLUS, $1, $3, null_value());}
     | expr TMINUS expr {$$ = make_node(MINUS, $1, $3, null_value());}
     | expr TMUL expr {$$ = make_node(MUL, $1, $3, null_value());}
     | expr TDIV expr {$$ = make_node(DIV, $1, $3, null_value());}
     | expr TCEQ expr {$$ = make_node(CEQ, $1, $3, null_value());}
     | expr TCLT expr {$$ = make_node(CLT, $1, $3, null_value());}
     | expr TCLE expr {$$ = make_node(CLTE, $1, $3, null_value());}
     | expr TCGT expr {$$ = make_node(CGT, $1, $3, null_value());}
     | expr TCGE expr {$$ = make_node(CGTE, $1, $3, null_value());}
     | TLBRACE stmt_list TRBRACE {$$ = make_node(BLOCK, $2, NULL, null_value());}
     | TIF expr expr {$$ = make_node(IF, $2, $3, null_value());}
     | TWHILE expr expr {$$ = make_node(WHILE, $2, $3, null_value());}
     | TFN TLPAREN param_list TRPAREN expr {$$ = make_node(FN, $3, $5, null_value());}
     | TCLASS TLPAREN TRPAREN expr {$$ = make_node(CLASS, NULL, $4, null_value());}
     | TCLASS TLPAREN expr TRPAREN expr {$$ = make_node(CLASS, $3, $5, null_value());} 
     | TRETURN expr {$$ = make_node(RETURN, $2, NULL, null_value());}
     ;

constant : TSTRING {$$ = $1;}
	 | TINTEGER {$$ = $1;}
	 | TTRUE {$$ = const_bool_node(1);}
	 | TFALSE {$$ = const_bool_node(0);}
	 ;

identifier : TIDENTIFIER {$$ = $1;}
	   ;

param_list : /* blank */ {$$ = make_node(PARAM_LIST, NULL, NULL, null_value());}
	   | TIDENTIFIER {$$ = make_node(PARAM_LIST, $1, NULL, null_value());}
	   | param_list TCOMMA TIDENTIFIER {$$ = make_node(PARAM_LIST, $3, $1, null_value());}
	   ;

func_args : /* blank */ {$$ = make_node(FUNC_ARGS, NULL, NULL, null_value());}
	  | expr {$$ = make_node(FUNC_ARGS, $1, NULL, null_value());}
	  | func_args TCOMMA expr {$$ = make_node(FUNC_ARGS, $3, $1, null_value());}
	  ;

%%
