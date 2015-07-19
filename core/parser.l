%{
#include "scanner_state.h"
#include "node.h"
#include <stdio.h>
#include <stddef.h>
#include <cuttle/debug.h>

#include "parser.h"  /* This is where it gets the definition for yylloc from */
#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno;

int f(int token, yyscan_t scanner);

%}

%option outfile="lexer.c" header-file="lexer.h"

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge bison-locations
%option yylineno
%option extra-type="struct scanner_state *"

%%

[ \t\r\n]               {}
#.*                     {}
"if"                    {return TIF;}
"while"                 {return TWHILE;}
"fn"                    {return TFN;}
"ns"                    {return TNS;}
"return"                {return TRETURN;}
"true"                  {return TTRUE;}
"false"                 {return TFALSE;}
"="                     {return TEQUALS;}
":="                    {return TGLOBAL;}
"@"                     {return TAT;}
"$"                     {return TDOLLAR;}
"("                     {return TLPAREN;}
")"                     {return TRPAREN;}
"["                     {return TLSQUARE;}
"]"                     {return TRSQUARE;}
\{|do                   {return TLBRACE;}
\}|end                  {return TRBRACE;}
-?[0-9]+                {yylval->node = solid_const_integer_node(atoi(yytext)); return TINTEGER;}
-?[0-9]+\.[0-9]+        {yylval->node = solid_const_double_node(atof(yytext)); return TDOUBLE;}
"~"                     {return TTILDE;}
"."                     {return TDOT;}
","                     {return TCOMMA;}
";"                     {return TSEMICOLON;}
(-|[+*/<>=^&%:!])*      {yylval->node = solid_identifier_node(yytext); return TINLINE_IDENTIFIER;}
[a-zA-Z_][a-zA-Z0-9_]*  {yylval->node = solid_identifier_node(yytext); return TIDENTIFIER;}
\"([^\\\"]|\\.)*\"      {yytext[strlen(yytext) - 1] = '\0'; yylval->node = solid_const_string_node(yytext+1); return TSTRING;}
.                       {log_err("Unknown token %s", yytext); yyterminate();}

%%
