%{
#include "node.h"
#include "parser.h"
#include <stdio.h>
%}

%option outfile="lexer.c" header-file="lexer.h"

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

%%

[ \t\n\r]               {}
"if"                    {return TIF;}
"while"                 {return TWHILE;}
"true"                  {return TTRUE;}
"false"                 {return TFALSE;}
[a-zA-Z_][a-zA-Z0-9_]*  {yylval->node = identifier_node(yytext); return TIDENTIFIER;}
L?\"(\\.|[^\\"])*\"     {yylval->node = const_string_node(yytext); return TSTRING;}
[0-9]+                  {yylval->node = const_integer_node(atoi(yytext)); return TINTEGER;}
"="                     {return TEQUAL;}
"=="                    {return TCEQ;}
"<"                     {return TCLT;}
"<="                    {return TCLE;}
">"                     {return TCGT;}
">="                    {return TCGE;}
"("                     {return TLPAREN;}
")"                     {return TRPAREN;}
"["                     {return TLSQUARE;}
"]"                     {return TRSQUARE;}
"{"                     {return TLBRACE;}
"}"                     {return TRBRACE;}
"."                     {return TDOT;}
","                     {return TCOMMA;}
";"                     {return TSEMICOLON;}
"+"                     {return TPLUS;}
"-"                     {return TMINUS;}
"*"                     {return TMUL;}
"/"                     {return TDIV;}
.                       {printf("Unknown token!\n"); yyterminate();}

%%

int yyerror(const char *msg) {
	fprintf(stderr,"Error:%s\n",msg); return 0;
}
