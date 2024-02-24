/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

#ifndef YY_YY_CCL_MASTER_PARSER_HPP_INCLUDED
# define YY_YY_CCL_MASTER_PARSER_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     t_SEMICOLON = 258,
     t_INTEGER = 259,
     t_REAL = 260,
     t_EQUALS = 261,
     t_LCURLY = 262,
     t_RCURLY = 263,
     t_COMMA = 264,
     t_TRUE = 265,
     t_FALSE = 266,
     t_NOT = 267,
     t_LBBRACKET = 268,
     t_RBBRACKET = 269,
     t_LBRACKET = 270,
     t_RBRACKET = 271,
     t_HEAD = 272,
     t_TAIL = 273,
     t_CONCAT = 274,
     t_CONS = 275,
     t_DOT = 276,
     t_ASSIGN = 277,
     t_LAMBDA = 278,
     t_COLON = 279,
     t_STRCAT = 280,
     t_COMPOSE = 281,
     t_NEEDS = 282,
     t_VAR = 283,
     t_STRING = 284,
     t_OR = 285,
     t_AND = 286,
     t_LESS_THAN = 287,
     t_LESS_THAN_OR_EQUAL = 288,
     t_GREATER_THAN = 289,
     t_GREATER_THAN_OR_EQUAL = 290,
     t_EQUAL = 291,
     t_NOT_EQUAL = 292,
     t_MINUS = 293,
     t_MOD = 294,
     t_DIV = 295,
     t_POWER = 296,
     t_RECORD_ADD = 297,
     t_PLUS = 298,
     t_MULT = 299,
     t_LPAREN = 300,
     t_RPAREN = 301,
     t_APP = 302,
     t_LET = 303,
     t_IN = 304,
     t_FOREACH = 305,
     t_DO = 306,
     t_END = 307,
     t_IF = 308,
     t_THEN = 309,
     t_ELSE = 310,
     t_FUN = 311,
     t_EXTERNAL = 312,
     t_INTERNAL = 313,
     t_PROGRAM = 314,
     t_SHARING = 315,
     t_EXEC = 316,
     t_BACKQUOTE = 317,
     t_UNIT = 318,
     t_BOOL = 319,
     t_INT = 320,
     t_LIST = 321,
     t_VARARGS = 322,
     t_INTO = 323,
     t_MAP_TO_CELLS = 324,
     UNOT = 325,
     UMINUS = 326
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 172 "../ccl-master/parser.y"


  Expr * exp;
  char * str;        /* for variable names */
  int i;
  double real;
  std::list<Expr *> * elist;
  std::list<Expr::FIELD *> * field_list;
  std::list<char *> * vlist;
  Command * command;
  std::list<Command *> * command_list;
  Clause * cls;
  Statement * st;
  std::list<Statement *> * st_list;
  Program * prog;  
  TypeExpr * type;
  std::list<TypeExpr *> * type_list;
  Environment * env;



/* Line 2058 of yacc.c  */
#line 150 "../ccl-master/parser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_CCL_MASTER_PARSER_HPP_INCLUDED  */
