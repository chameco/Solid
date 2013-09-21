%{
#include "scanner_state.h"
#include "node.h"
#include <stdio.h>

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

[ \t\r]                 {}
"if"                    {int temp = f(TIF, yyscanner); if (temp) return temp;}
"while"                 {int temp = f(TWHILE, yyscanner); if (temp) return temp;}
"fn"                    {int temp = f(TFN, yyscanner); if (temp) return temp;}
"ns"                    {int temp = f(TNS, yyscanner); if (temp) return temp;}
"return"                {int temp = f(TRETURN, yyscanner); if (temp) return temp;}
"true"                  {int temp = f(TTRUE, yyscanner); if (temp) return temp;}
"false"                 {int temp = f(TFALSE, yyscanner); if (temp) return temp;}
"="                     {int temp = f(TEQUALS, yyscanner); if (temp) return temp;}
"("                     {int temp = f(TLPAREN, yyscanner); if (temp) return temp;}
")"                     {int temp = f(TRPAREN, yyscanner); if (temp) return temp;}
"["                     {int temp = f(TLSQUARE, yyscanner); if (temp) return temp;}
"]"                     {int temp = f(TRSQUARE, yyscanner); if (temp) return temp;}
\{|do                   {int temp = f(TLBRACE, yyscanner); if (temp) return temp;}
\}|end                  {int temp = f(TRBRACE, yyscanner); if (temp) return temp;}
"."                     {int temp = f(TDOT, yyscanner); if (temp) return temp;}
","                     {int temp = f(TCOMMA, yyscanner); if (temp) return temp;}
";"                     {int temp = f(TSEMICOLON, yyscanner); if (temp) return temp;}
"\n"                    {int temp = f(TNEWLINE, yyscanner); if (temp) return temp;}
(-|[+*/<>=^&%:!])*      {yylval->node = identifier_node(yytext); int temp = f(TINLINE_IDENTIFIER, yyscanner); if (temp) return temp;}
[a-zA-Z_][a-zA-Z0-9_]*  {yylval->node = identifier_node(yytext); int temp = f(TIDENTIFIER, yyscanner); if (temp) return temp;}
\"([^\\\"]|\\.)*\"      {yytext[strlen(yytext) - 1] = '\0'; yylval->node = const_string_node(yytext+1); int temp = f(TSTRING, yyscanner); if (temp) return temp;}
[0-9]+                  {yylval->node = const_integer_node(atoi(yytext)); int temp = f(TINTEGER, yyscanner); if (temp) return temp;}
.                       {log_err("Unknown token %s", yytext); yyterminate();}

%%

int f(int token, yyscan_t scanner) {
	struct scanner_state *extra = yyget_extra(scanner);
	if (extra->insert && token == TNEWLINE) {
		yyunput('\n', yyget_text(scanner), scanner);
		extra->insert = 0;
		return TSEMICOLON;
	} else if (token == TNEWLINE) {
		return 0;
	} else {
		extra->insert =
			((token == TIDENTIFIER) ||
			 (token == TINLINE_IDENTIFIER) ||
			 (token == TTRUE) ||
			 (token == TFALSE) ||
			 (token == TINTEGER) ||
			 (token == TSTRING) ||
			 (token == TRETURN) ||
			 (token == TRPAREN) ||
			 (token == TRSQUARE) ||
			 (token == TRBRACE));
		return token;
	}
}
