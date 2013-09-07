%{
#include "node.h"
#include <stdio.h>

#include "parser.h"  /* This is where it gets the definition for yylloc from */
#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno;

%}

%option outfile="lexer.c" header-file="lexer.h"

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge bison-locations
%option yylineno

%%

[ \t\n\r]               {}
"if"                    {return TIF;}
"while"                 {return TWHILE;}
"def"                   {return TDEF;}
"fn"                    {return TFN;}
"return"                {return TRETURN;}
"class"                 {return TCLASS;}
"new"                   {return TNEW;}
"true"                  {return TTRUE;}
"false"                 {return TFALSE;}
"=="                    {return TCEQ;}
"<"                     {return TCLT;}
"<="                    {return TCLE;}
">"                     {return TCGT;}
">="                    {return TCGE;}
"("                     {return TLPAREN;}
")"                     {return TRPAREN;}
"["                     {return TLSQUARE;}
"]"                     {return TRSQUARE;}
\{|do                   {return TLBRACE;}
\}|end                  {return TRBRACE;}
"."                     {return TDOT;}
","                     {return TCOMMA;}
";"                     {return TSEMICOLON;}
"+"                     {return TPLUS;}
"-"                     {return TMINUS;}
"*"                     {return TMUL;}
"/"                     {return TDIV;}
[a-zA-Z_][a-zA-Z0-9_]*  {yylval->node = identifier_node(yytext); return TIDENTIFIER;}
\"([^\\\"]|\\.)*\"      {yytext[strlen(yytext) - 1] = '\0'; yylval->node = const_string_node(yytext+1); return TSTRING;}
[0-9]+                  {yylval->node = const_integer_node(atoi(yytext)); return TINTEGER;}
.                       {log_err("Unknown token"); yyterminate();}

%%
