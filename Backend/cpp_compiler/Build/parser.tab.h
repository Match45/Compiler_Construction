/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     KEYWORD_NAMESPACE = 258,
     KEYWORD_USING = 259,
     KEYWORD_INT = 260,
     KEYWORD_FLOAT = 261,
     KEYWORD_DOUBLE = 262,
     KEYWORD_CHAR = 263,
     KEYWORD_VOID = 264,
     KEYWORD_IF = 265,
     KEYWORD_ELSE = 266,
     KEYWORD_WHILE = 267,
     KEYWORD_FOR = 268,
     KEYWORD_DO = 269,
     KEYWORD_BREAK = 270,
     KEYWORD_CONTINUE = 271,
     KEYWORD_RETURN = 272,
     KEYWORD_COUT = 273,
     KEYWORD_CIN = 274,
     KEYWORD_ENDL = 275,
     IDENTIFIER = 276,
     STRING_LITERAL = 277,
     NUMBER = 278,
     ASSIGNMENT = 279,
     RELATIONAL = 280,
     LOGICAL_AND = 281,
     LOGICAL_OR = 282,
     LOGICAL_NOT = 283,
     LEFT_SHIFT = 284,
     RIGHT_SHIFT = 285,
     INCREMENT = 286,
     DECREMENT = 287,
     SEMICOLON = 288,
     COMMA = 289,
     LBRACE = 290,
     RBRACE = 291,
     LPAREN = 292,
     RPAREN = 293,
     UMINUS = 294
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 27 "src/Parser/parser.y"

    char* str_val;
    ASTNode* node_val;
    std::vector<ASTNode*>* vec_val;
    std::vector<Parameter>* param_vec_val;
    Parameter* param_val;
    CoutItem* cout_item_val;
    std::vector<CoutItem>* cout_vec_val;
    std::vector<std::string>* str_vec_val;



/* Line 1685 of yacc.c  */
#line 103 "build/parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


