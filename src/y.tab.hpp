/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_HPP_INCLUDED
# define YY_YY_Y_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    UNUM = 258,                    /* UNUM  */
    ID = 259,                      /* ID  */
    ADD = 260,                     /* ADD  */
    SUB = 261,                     /* SUB  */
    MUL = 262,                     /* MUL  */
    DIV = 263,                     /* DIV  */
    CEQ = 264,                     /* CEQ  */
    NEQ = 265,                     /* NEQ  */
    GT = 266,                      /* GT  */
    GTE = 267,                     /* GTE  */
    LT = 268,                      /* LT  */
    LTE = 269,                     /* LTE  */
    AND = 270,                     /* AND  */
    OR = 271,                      /* OR  */
    NOT = 272,                     /* NOT  */
    EQ = 273,                      /* EQ  */
    LPAR = 274,                    /* LPAR  */
    RPAR = 275,                    /* RPAR  */
    LSQ = 276,                     /* LSQ  */
    RSQ = 277,                     /* RSQ  */
    LBRA = 278,                    /* LBRA  */
    RBRA = 279,                    /* RBRA  */
    COMMA = 280,                   /* COMMA  */
    SEMI = 281,                    /* SEMI  */
    RARROW = 282,                  /* RARROW  */
    COLON = 283,                   /* COLON  */
    DOT = 284,                     /* DOT  */
    LET = 285,                     /* LET  */
    FN = 286,                      /* FN  */
    RET = 287,                     /* RET  */
    INT = 288,                     /* INT  */
    STRUCT = 289,                  /* STRUCT  */
    IF = 290,                      /* IF  */
    ELSE = 291,                    /* ELSE  */
    WHILE = 292,                   /* WHILE  */
    BREAK = 293,                   /* BREAK  */
    CONTINUE = 294,                /* CONTINUE  */
    UMINUS = 295                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define UNUM 258
#define ID 259
#define ADD 260
#define SUB 261
#define MUL 262
#define DIV 263
#define CEQ 264
#define NEQ 265
#define GT 266
#define GTE 267
#define LT 268
#define LTE 269
#define AND 270
#define OR 271
#define NOT 272
#define EQ 273
#define LPAR 274
#define RPAR 275
#define LSQ 276
#define RSQ 277
#define LBRA 278
#define RBRA 279
#define COMMA 280
#define SEMI 281
#define RARROW 282
#define COLON 283
#define DOT 284
#define LET 285
#define FN 286
#define RET 287
#define INT 288
#define STRUCT 289
#define IF 290
#define ELSE 291
#define WHILE 292
#define BREAK 293
#define CONTINUE 294
#define UMINUS 295

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 19 "parser.yacc"

    // Program
    A_program program;
    A_programElement programElement;
    A_programElementList programElementList;
    // Arithmatic Expressions
    A_arithExpr arithExpr;
    A_exprUnit exprUnit;
    A_arithBiOpExpr arithBiOpExpr;
    A_arithUExpr arithUExpr;
    // Condition Expressions
    A_boolExpr boolExpr;
    A_boolUnit boolUnit;
    A_boolBiOpExpr boolBiOpExpr;
    A_boolUOpExpr boolUOpExpr;
    A_comExpr comExpr;
    // Assignment
    A_assignStmt assignStmt;
    A_leftVal leftVal;
    A_rightVal rightVal;
    // Function Call
    A_fnCall fnCall;
    // Variable Declarations
    A_varDeclStmt varDeclStmt;
    A_varDecl varDecl;
    A_varDef varDef;
    A_varDeclScalar varDeclScalar;
    A_varDeclArray varDeclArray;
    A_varDefScalar varDefScalar;
    A_varDefArray varDefArray;
    A_type type;
    // Define A New Structure
    A_structDef structDef;
    // Function Declaration
    A_fnDeclStmt fnDeclStmt;
    A_fnDecl fnDecl;
    A_paramDecl paramDecl;
    // Function Definition
    A_fnDef fnDef;
    A_codeBlockStmt codeBlockStmt;
    A_codeBlockStmtList codeBlockStmtList;
    A_returnStmt returnStmt;
    A_callStmt callStmt;
    // Control Flows
    A_ifStmt ifStmt;
    A_whileStmt whileStmt;
    // Others
    A_pos pos;
    A_tokenId tokenId;
    A_tokenNum tokenNum;
    A_arrayExpr arrayExpr;
    A_memberExpr memberExpr;
    A_rightValList rightValList;
    A_varDeclList varDeclList;
    // A_indexExpr indexExpr;

#line 204 "y.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_HPP_INCLUDED  */
