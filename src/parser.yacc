%{
#include <stdio.h>
#include "TeaplAst.h"

extern A_pos pos;
extern A_program root;

extern int yylex(void);
extern "C"{
extern void yyerror(char *s); 
extern int  yywrap();
}

%}

// TODO:
// your parser

%union {
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
}

// token
%token <tokenNum> UNUM
%token <tokenId> ID 

%token <pos> ADD
%token <pos> SUB
%token <pos> MUL
%token <pos> DIV

%token <pos> CEQ    // ==
%token <pos> NEQ    // !=
%token <pos> GT     // >
%token <pos> GTE    // >=
%token <pos> LT     // <
%token <pos> LTE    // <=

%token <pos> AND
%token <pos> OR
%token <pos> NOT

%token <pos> EQ     // =

%token <pos> LPAR   // (
%token <pos> RPAR   // )
%token <pos> LSQ    // [
%token <pos> RSQ    // ]
%token <pos> LBRA   // {
%token <pos> RBRA   // }

%token <pos> COMMA  // ,
%token <pos> SEMI   // ;
%token <pos> RARROW // ->
%token <pos> COLON  // :
%token <pos> DOT    // .

%token <pos> LET
%token <pos> FN
%token <pos> RET
%token <pos> INT
%token <pos> STRUCT
%token <pos> IF
%token <pos> ELSE
%token <pos> WHILE
%token <pos> BREAK
%token <pos> CONTINUE

// type
// Program
%type <program> Program
%type <programElement> ProgramElement
%type <programElementList> ProgramElementList
// Arithmatic Expressions
%type <arithExpr> ArithExpr
%type <exprUnit> ExprUnit
%type <arithBiOpExpr> ArithBiOpExpr
%type <arithUExpr> ArithUExpr
// Condition Expressions
%type <boolExpr> BoolExpr
%type <boolBiOpExpr> BoolBiOpExpr
%type <boolUnit> BoolUnit
%type <boolUOpExpr> BoolUOpExpr
%type <comExpr> ComExpr
// Assignment
%type <assignStmt> AssignStmt
%type <leftVal> LeftVal
%type <rightVal> RightVal
// Function Call
%type <fnCall> FnCall
// Variable Declarations
%type <varDeclStmt> VarDeclStmt
%type <varDecl> VarDecl
%type <varDef> VarDef
%type <varDeclScalar> VarDeclScalar
%type <varDeclArray> VarDeclArray
%type <varDefScalar> VarDefScalar
%type <varDefArray> VarDefArray
%type <type> Type
// Define A New Structure
%type <structDef> StructDef
// Function Declaration
%type <fnDeclStmt> FnDeclStmt
%type <fnDecl> FnDecl
%type <paramDecl> ParamDecl
// Function Definition
%type <fnDef> FnDef
%type <codeBlockStmt> CodeBlockStmt
%type <codeBlockStmtList> CodeBlockStmtList
%type <returnStmt> ReturnStmt
%type <callStmt> CallStmt
// Control Flows
%type <ifStmt> IfStmt
%type <whileStmt> WhileStmt
// Others
%type <arrayExpr> ArrayExpr
%type <memberExpr> MemberExpr
%type <rightValList> RightValList
%type <varDeclList> VarDeclList
// %type <indexExpr> IndexExpr

// Operator Precedence
%right EQ
%left OR
%left AND
%left CEQ NEQ
%left GT GTE LT LTE
%left ADD SUB
%left MUL DIV
%right UMINUS NOT
%left LPAR RPAR LSQ RSQ DOT RARROW

%start Program

%%                   /* beginning of rules section */

Program: ProgramElementList 
{  
  root = A_Program($1);
  $$ = A_Program($1);
}
;

ProgramElementList: ProgramElement ProgramElementList
{
  $$ = A_ProgramElementList($1, $2);
}
|
{
  $$ = nullptr;
}
;

ProgramElement: VarDeclStmt
{
  $$ = A_ProgramVarDeclStmt($1->pos, $1);
}
| StructDef
{
  $$ = A_ProgramStructDef($1->pos, $1);
}
| FnDeclStmt
{
  $$ = A_ProgramFnDeclStmt($1->pos, $1);
}
| FnDef
{
  $$ = A_ProgramFnDef($1->pos, $1);
}
| SEMI
{
  $$ = A_ProgramNullStmt($1);
}
;

ArithExpr: ArithBiOpExpr
{
  $$ = A_ArithBiOp_Expr($1->pos, $1);
}
| ExprUnit
{
  $$ = A_ExprUnit($1->pos, $1);
}
;

ArithBiOpExpr: ArithExpr ADD ArithExpr
{
  $$ = A_ArithBiOpExpr($1->pos, A_add, $1, $3);
}
| ArithExpr SUB ArithExpr
{
  $$ = A_ArithBiOpExpr($1->pos, A_sub, $1, $3);
}
| ArithExpr MUL ArithExpr
{
  $$ = A_ArithBiOpExpr($1->pos, A_mul, $1, $3);
}
| ArithExpr DIV ArithExpr
{
  $$ = A_ArithBiOpExpr($1->pos, A_div, $1, $3);
}
;

ExprUnit: UNUM
{
  $$ = A_NumExprUnit($1->pos, $1->num);
}
| ID
{
  $$ = A_IdExprUnit($1->pos, $1->id);
}
| FnCall
{
  $$ = A_CallExprUnit($1->pos, $1);
}
| ArrayExpr
{
  $$ = A_ArrayExprUnit($1->pos, $1);
}
| MemberExpr
{
  $$ = A_MemberExprUnit($1->pos, $1);
}
| ArithUExpr
{
  $$ = A_ArithUExprUnit($1->pos, $1);
}
| LPAR ArithExpr RPAR
{
  $$ = A_ArithExprUnit($1, $2);
}
;

ArithUExpr: SUB ExprUnit %prec UMINUS
{
  $$ = A_ArithUExpr($1, A_neg, $2);
}
;

BoolExpr: BoolBiOpExpr
{
  $$ = A_BoolBiOp_Expr($1->pos, $1);
}
| BoolUnit
{
  $$ = A_BoolExpr($1->pos, $1);
}
;

BoolBiOpExpr: BoolExpr AND BoolExpr
{
  $$ = A_BoolBiOpExpr($1->pos, A_and, $1, $3);
}
| BoolExpr OR BoolExpr
{
  $$ = A_BoolBiOpExpr($1->pos, A_or, $1, $3);
}
;

BoolUnit: BoolUOpExpr
{
  $$ = A_BoolUOpExprUnit($1->pos, $1);
}
| ComExpr
{
  $$ = A_ComExprUnit($1->pos, $1);
}
| LPAR BoolExpr RPAR
{
  $$ = A_BoolExprUnit($1, $2);
}
;

BoolUOpExpr: NOT BoolUnit
{
  $$ = A_BoolUOpExpr($1, A_not, $2);
}
;

ComExpr: ExprUnit GT ExprUnit
{
  $$ = A_ComExpr($1->pos, A_gt, $1, $3);
}
| ExprUnit GTE ExprUnit
{
  $$ = A_ComExpr($1->pos, A_ge, $1, $3);
}
| ExprUnit LT ExprUnit
{
  $$ = A_ComExpr($1->pos, A_lt, $1, $3);
}
| ExprUnit LTE ExprUnit
{
  $$ = A_ComExpr($1->pos, A_le, $1, $3);
}
| ExprUnit CEQ ExprUnit
{
  $$ = A_ComExpr($1->pos, A_eq, $1, $3);
}
| ExprUnit NEQ ExprUnit
{
  $$ = A_ComExpr($1->pos, A_ne, $1, $3);
}
;

AssignStmt: LeftVal EQ RightVal SEMI
{
  $$ = A_AssignStmt($1->pos, $1, $3);
}
;

LeftVal: ID
{
  $$ = A_IdExprLVal($1->pos, $1->id);
}
| ArrayExpr
{
  $$ = A_ArrExprLVal($1->pos, $1);
}
| MemberExpr
{
  $$ = A_MemberExprLVal($1->pos, $1);
}
;

RightVal: ArithExpr
{
  $$ = A_ArithExprRVal($1->pos, $1);
}
| BoolExpr
{
  $$ = A_BoolExprRVal($1->pos, $1);
}
;

FnCall: ID LPAR RightValList RPAR
{
  $$ = A_FnCall($1->pos, $1->id, $3);
}

VarDeclStmt: LET VarDecl SEMI
{
  $$ = A_VarDeclStmt($1, $2);
}
| LET VarDef SEMI
{
  $$ = A_VarDefStmt($1, $2);
}
;

VarDecl: VarDeclScalar
{
  $$ = A_VarDecl_Scalar($1->pos, $1);
}
| VarDeclArray
{
  $$ = A_VarDecl_Array($1->pos, $1);
}
;

VarDeclScalar: ID COLON Type
{
  $$ = A_VarDeclScalar($1->pos, $1->id, $3);
}
| ID
{
  $$ = A_VarDeclScalar($1->pos, $1->id, nullptr);
}
;

VarDeclArray: ID LSQ UNUM RSQ COLON Type
{
  $$ = A_VarDeclArray($1->pos, $1->id, $3->num, $6);
}
| ID LSQ UNUM RSQ
{
  $$ = A_VarDeclArray($1->pos, $1->id, $3->num, nullptr);
}
;

VarDef: VarDefScalar
{
  $$ = A_VarDef_Scalar($1->pos, $1);
}
| VarDefArray
{
  $$ = A_VarDef_Array($1->pos, $1);
}
;

VarDefScalar: ID EQ RightVal
{
  $$ = A_VarDefScalar($1->pos, $1->id, nullptr, $3);
}
| ID COLON Type EQ RightVal
{
  $$ = A_VarDefScalar($1->pos, $1->id, $3, $5);
}
;

VarDefArray: ID LSQ UNUM RSQ EQ LBRA RightValList RBRA
{
  $$ = A_VarDefArray($1->pos, $1->id, $3->num, nullptr, $7);
}
| ID LSQ UNUM RSQ COLON Type EQ LBRA RightValList RBRA
{
  $$ = A_VarDefArray($1->pos, $1->id, $3->num, $6, $9);
}
;

Type: INT 
{
  $$ = A_NativeType($1, A_intTypeKind);
}
| ID
{
  $$ = A_StructType($1->pos, $1->id);
}
;

StructDef: STRUCT ID LBRA VarDeclList RBRA
{
  $$ = A_StructDef($1, $2->id, $4);
}
;

FnDeclStmt: FnDecl SEMI
{
  $$ = A_FnDeclStmt($1->pos, $1);
}
;

FnDecl: FN ID LPAR ParamDecl RPAR 
{
  $$ = A_FnDecl($1, $2->id, $4, nullptr);
}
| FN ID LPAR ParamDecl RPAR RARROW Type
{
  $$ = A_FnDecl($1, $2->id, $4, $7);
}
;

ParamDecl: VarDeclList
{
  $$ = A_ParamDecl($1);
}
;

FnDef: FnDecl LBRA CodeBlockStmtList RBRA
{
  $$ = A_FnDef($1->pos, $1, $3);
}
;

CodeBlockStmtList: CodeBlockStmt CodeBlockStmtList
{
  $$ = A_CodeBlockStmtList($1, $2);
}
|
{
  $$ = NULL;
}
;

CodeBlockStmt: VarDeclStmt
{
  $$ = A_BlockVarDeclStmt($1->pos, $1);
}
| AssignStmt
{
  $$ = A_BlockAssignStmt($1->pos, $1);
}
| CallStmt
{
  $$ = A_BlockCallStmt($1->pos, $1);
}
| IfStmt
{
  $$ = A_BlockIfStmt($1->pos, $1);
}
| WhileStmt
{
  $$ = A_BlockWhileStmt($1->pos, $1);
}
| ReturnStmt
{
  $$ = A_BlockReturnStmt($1->pos, $1);
}
| CONTINUE SEMI
{
  $$ = A_BlockContinueStmt($1);
}
| BREAK SEMI
{
  $$ = A_BlockBreakStmt($1);
}
| SEMI
{
  $$ = A_BlockNullStmt($1);
}
;

ReturnStmt: RET RightVal SEMI
{
  $$ = A_ReturnStmt($1, $2);
}
| RET SEMI
{
  $$ = A_ReturnStmt($1, nullptr);
}
;

CallStmt: FnCall SEMI
{
  $$ = A_CallStmt($1->pos, $1);
}
;

IfStmt: IF LPAR BoolExpr RPAR LBRA CodeBlockStmtList RBRA ELSE LBRA CodeBlockStmtList RBRA
{
  $$ = A_IfStmt($1, $3, $6, $10);
}
| IF LPAR BoolExpr RPAR LBRA CodeBlockStmtList RBRA
{
  $$ = A_IfStmt($1, $3, $6, nullptr);
}
;

WhileStmt: WHILE LPAR BoolExpr RPAR LBRA CodeBlockStmtList RBRA
{
  $$ = A_WhileStmt($1, $3, $6);
}
;

ArrayExpr: LeftVal LSQ UNUM RSQ
{
  $$ = A_ArrayExpr($1->pos, $1, A_NumIndexExpr($3->pos, $3->num));
}
| LeftVal LSQ ID RSQ
{
  $$ = A_ArrayExpr($1->pos, $1, A_IdIndexExpr($3->pos, $3->id));
}
;

MemberExpr: LeftVal DOT ID
{
  $$ = A_MemberExpr($1->pos, $1, $3->id);
}
;

RightValList: RightVal COMMA RightValList
{
  $$ = A_RightValList($1, $3);
}
| RightVal
{
  $$ = A_RightValList($1, nullptr);
}
|
{
  $$ = nullptr;
}
;

VarDeclList: VarDecl COMMA VarDeclList
{
  $$ = A_VarDeclList($1, $3);
}
| VarDecl
{
  $$ = A_VarDeclList($1, nullptr);
}
|
{
  $$ = nullptr;
}
;

%%

extern "C"{
void yyerror(char * s)
{
  fprintf(stderr, "%s\n",s);
}
int yywrap()
{
  return(1);
}
}

