%{

#define _GNU_SOURCE
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "../Parser/ast.h"
#include "../Parser/parser.tab.h"

inline char* my_strdup(const char* s) {
    if (!s) return nullptr;
    size_t len = strlen(s) + 1;
    char* dup = (char*)malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}
#define strdup my_strdup

using namespace std;

%}

%option noyywrap
%option yylineno
%option never-interactive

%%

"#".*     { /* Ignore preprocessor directives */ }

"namespace"     { return KEYWORD_NAMESPACE; }
"using"         { return KEYWORD_USING; }
"int"           { yylval.str_val = strdup(yytext); return KEYWORD_INT; }
"float"         { yylval.str_val = strdup(yytext); return KEYWORD_FLOAT; }
"double"        { yylval.str_val = strdup(yytext); return KEYWORD_DOUBLE; }
"char"          { yylval.str_val = strdup(yytext); return KEYWORD_CHAR; }
"void"          { yylval.str_val = strdup(yytext); return KEYWORD_VOID; }
"if"            { return KEYWORD_IF; }
"else"          { return KEYWORD_ELSE; }
"while"         { return KEYWORD_WHILE; }
"for"           { return KEYWORD_FOR; }
"do"            { return KEYWORD_DO; }
"break"         { return KEYWORD_BREAK; }
"continue"      { return KEYWORD_CONTINUE; }
"cout"          { return KEYWORD_COUT; }
"cin"           { return KEYWORD_CIN; }
"endl"          { return KEYWORD_ENDL; }
"return"        { return KEYWORD_RETURN; }

\"([^\\\"]|\\.)*\"              { yylval.str_val = strdup(yytext); return STRING_LITERAL; }

[0-9]+(\.[0-9]+)?               { yylval.str_val = strdup(yytext); return NUMBER; }

[a-zA-Z_][a-zA-Z0-9_]*          { yylval.str_val = strdup(yytext); return IDENTIFIER; }

"=="|"!="|"<="|">="|"<"|">"     { yylval.str_val = strdup(yytext); return RELATIONAL; }

"//".*                      { }

"/*"([^*]|\*+[^*/])*\*+"/"  { }

"<<"        { return LEFT_SHIFT; }
">>"        { return RIGHT_SHIFT; }

"="         { return ASSIGNMENT; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"%"         { return '%'; }

"++"        { return INCREMENT; }
"--"        { return DECREMENT; }

"&&"        { return LOGICAL_AND; }
"||"        { return LOGICAL_OR; }
"!"         { return LOGICAL_NOT; }

";"         { return SEMICOLON; }
","         { return COMMA; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"("         { return LPAREN; }
")"         { return RPAREN; }

[ \t\r]+    { }

\n          { }

. {
    cout << "Lexer Warning on Line " << yylineno << ": Unknown character -> " << yytext << endl;
}

%%