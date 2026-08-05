%{
#include "parser.tab.h"
#include "../src/ast/ast.h"

#if defined(__MINGW32__) || defined(_WIN32)
  #include <io.h>
  #include <string.h>
#endif

extern int yylineno;
bool showTokens = false;

void printToken(const char* name, const char* text) {
    if (showTokens) printf("  [%-14s] %s\n", name, text);
}
%}

%option noyywrap
%option yylineno

%option noyywrap
%option yylineno

DIGIT     [0-9]
LETTER    [a-zA-Z_]
ID        {LETTER}({LETTER}|{DIGIT})*
INT       {DIGIT}+
FLOAT     {DIGIT}+"."{DIGIT}+
STRING    \"([^\\\"]|\\.)*\"
CHARLIT   \'([^\\\']|\\.)\'

%%

"//".*                         { /* line comment - skip */ }
"/*"([^*]|\*+[^*/])*\*+"/"     { /* block comment - skip */ }

"int"       { printToken("KEYWORD", yytext); return INT_T; }
"float"     { printToken("KEYWORD", yytext); return FLOAT_T; }
"char"      { printToken("KEYWORD", yytext); return CHAR_T; }
"double"    { printToken("KEYWORD", yytext); return DOUBLE_T; }
"void"      { printToken("KEYWORD", yytext); return VOID_T; }
"if"        { printToken("KEYWORD", yytext); return IF; }
"else"      { printToken("KEYWORD", yytext); return ELSE; }
"while"     { printToken("KEYWORD", yytext); return WHILE; }
"for"       { printToken("KEYWORD", yytext); return FOR; }
"return"    { printToken("KEYWORD", yytext); return RETURN; }
"break"     { printToken("KEYWORD", yytext); return BREAK; }
"continue"  { printToken("KEYWORD", yytext); return CONTINUE; }

{ID}        { printToken("IDENTIFIER", yytext); yylval.strval = strdup(yytext); return ID; }
{FLOAT}     { printToken("FLOAT_CONST", yytext); yylval.floatval = atof(yytext); return FLOAT_CONST; }
{INT}       { printToken("INT_CONST", yytext); yylval.intval = atoi(yytext); return INT_CONST; }
{STRING}    { printToken("STRING_CONST", yytext); yylval.strval = strdup(yytext); return STRING_CONST; }
{CHARLIT}   { printToken("CHAR_CONST", yytext); yylval.strval = strdup(yytext); return CHAR_CONST; }

"=="        { printToken("OPERATOR", yytext); return EQ; }
"!="        { printToken("OPERATOR", yytext); return NEQ; }
"<="        { printToken("OPERATOR", yytext); return LE; }
">="        { printToken("OPERATOR", yytext); return GE; }
"&&"        { printToken("OPERATOR", yytext); return AND; }
"||"        { printToken("OPERATOR", yytext); return OR; }
"++"        { printToken("OPERATOR", yytext); return INC; }
"--"        { printToken("OPERATOR", yytext); return DEC; }

[-+*/%=<>!;,(){}\[\]]  { printToken("SYMBOL", yytext); return yytext[0]; }

[ \t\r]+    { /* skip whitespace */ }
\n          { /* yylineno auto-increments */ }

.           { fprintf(stderr, "Lexical Error: unexpected char '%s' at line %d\n", yytext, yylineno); }

%%