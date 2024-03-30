%{
#include <stdio.h>
#include <string.h>
#include "TeaplAst.h"
#include "y.tab.hpp"
extern int line, col;
int c;
int calc(char *s, int len);
char* str(char* s, int len);
%}
%start COMMENT1 COMMENT2

%%

<INITIAL>"//" { col += yyleng; BEGIN COMMENT1; }
<INITIAL>"/*" { col += yyleng; BEGIN COMMENT2; }

<COMMENT1>"\n" { ++line; col = 0; BEGIN INITIAL; }
<COMMENT1>.    { col += yyleng; }

<COMMENT2>"*/" { col += yyleng; BEGIN INITIAL; }
<COMMENT2>"\n" { ++line; col = 0; }
<COMMENT2>.    { col += yyleng; }

<INITIAL>"\n"          { line++; col = 0; }
<INITIAL>" "|"\r"|"\t" { col += yyleng; }

<INITIAL>"+" { yylval.pos = A_Pos(line, col); col += yyleng; return ADD; }
<INITIAL>"-" { yylval.pos = A_Pos(line, col); col += yyleng; return SUB; }
<INITIAL>"*" { yylval.pos = A_Pos(line, col); col += yyleng; return MUL; }
<INITIAL>"/" { yylval.pos = A_Pos(line, col); col += yyleng; return DIV; }

<INITIAL>">"  { yylval.pos = A_Pos(line, col); col += yyleng; return GT; }
<INITIAL>">=" { yylval.pos = A_Pos(line, col); col += yyleng; return GTE; }
<INITIAL>"<"  { yylval.pos = A_Pos(line, col); col += yyleng; return LT; }
<INITIAL>"<=" { yylval.pos = A_Pos(line, col); col += yyleng; return LTE; }
<INITIAL>"==" { yylval.pos = A_Pos(line, col); col += yyleng; return CEQ; }
<INITIAL>"!=" { yylval.pos = A_Pos(line, col); col += yyleng; return NEQ; }

<INITIAL>"&&" { yylval.pos = A_Pos(line, col); col += yyleng; return AND; }
<INITIAL>"||" { yylval.pos = A_Pos(line, col); col += yyleng; return OR; }
<INITIAL>"!"  { yylval.pos = A_Pos(line, col); col += yyleng; return NOT; }

<INITIAL>"=" { yylval.pos = A_Pos(line, col); col += yyleng; return EQ; }

<INITIAL>"(" { yylval.pos = A_Pos(line, col); col += yyleng; return LPAR; }
<INITIAL>")" { yylval.pos = A_Pos(line, col); col += yyleng; return RPAR; }
<INITIAL>"[" { yylval.pos = A_Pos(line, col); col += yyleng; return LSQ; }
<INITIAL>"]" { yylval.pos = A_Pos(line, col); col += yyleng; return RSQ; }
<INITIAL>"{" { yylval.pos = A_Pos(line, col); col += yyleng; return LBRA; }
<INITIAL>"}" { yylval.pos = A_Pos(line, col); col += yyleng; return RBRA; }

<INITIAL>","  { yylval.pos = A_Pos(line, col); col += yyleng; return COMMA; }
<INITIAL>";"  { yylval.pos = A_Pos(line, col); col += yyleng; return SEMI; }
<INITIAL>"->" { yylval.pos = A_Pos(line, col); col += yyleng; return RARROW; }
<INITIAL>":"  { yylval.pos = A_Pos(line, col); col += yyleng; return COLON; }
<INITIAL>"."  { yylval.pos = A_Pos(line, col); col += yyleng; return DOT; }

<INITIAL>"let"      { yylval.pos = A_Pos(line, col); col += yyleng; return LET; }
<INITIAL>"if"       { yylval.pos = A_Pos(line, col); col += yyleng; return IF; }
<INITIAL>"else"     { yylval.pos = A_Pos(line, col); col += yyleng; return ELSE; }
<INITIAL>"while"    { yylval.pos = A_Pos(line, col); col += yyleng; return WHILE; }
<INITIAL>"int"      { yylval.pos = A_Pos(line, col); col += yyleng; return INT; }
<INITIAL>"fn"       { yylval.pos = A_Pos(line, col); col += yyleng; return FN; }
<INITIAL>"ret"      { yylval.pos = A_Pos(line, col); col += yyleng; return RET; }
<INITIAL>"struct"   { yylval.pos = A_Pos(line, col); col += yyleng; return STRUCT; }
<INITIAL>"break"    { yylval.pos = A_Pos(line, col); col += yyleng; return BREAK; }
<INITIAL>"continue" { yylval.pos = A_Pos(line, col); col += yyleng; return CONTINUE; }

<INITIAL>0|[1-9][0-9]* {
    yylval.tokenNum = A_TokenNum(A_Pos(line, col), calc(yytext, yyleng));
    col+=yyleng;
    return UNUM;
}

<INITIAL>[a-z_A-Z][a-z_A-Z0-9]* { 
    yylval.tokenId = A_TokenId(A_Pos(line, col), str(yytext, yyleng));
    col += yyleng;
    return ID;
}

<INITIAL>. { 
    col += yyleng; 
    fprintf(stderr, "Illegal input \"%c\"\n", yytext[0]); 
    return 1; 
}

%%

// This function takes a string of digits and its length as input, and returns the integer value of the string.
int calc(char *s, int len) {
    int ret = 0;
    for(int i = 0; i < len; i++)
        ret = ret * 10 + (s[i] - '0');
    return ret;
}

char* str(char* s, int len) {
    char* p = (char*)malloc(len + 1);
    strcpy(p, s);
    return p;
}