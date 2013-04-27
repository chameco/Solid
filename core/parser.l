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

[ \t\r]                 {}
"\n"                    {return TNEWLINE;}
"="                     {return TEQUAL;}
"=="                    {return TCEQ;}
"!="                    {return TCNE;}
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
"+"                     {return TPLUS;}
"-"                     {return TMINUS;}
"*"                     {return TMUL;}
"/"                     {return TDIV;}
.                       {printf("Unknown token!\n"); yyterminate();}

%%

int yyerror(const char *msg) {
	fprintf(stderr,"Error:%s\n",msg); return 0;
}
