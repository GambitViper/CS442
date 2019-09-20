%{
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include "IOMngr.h"
  #include "Scanner.h"
  #include "YSemantics.h"

  void yyerror(char *s);

  #define MAKE_SPAN(loc) MakeSpan(loc.first_column,loc.last_column)
%}

%locations

/* Union structure of data that can be attached to non-terminals   */
/* type union-field-name                                           */
%union     {
  int Integer;
  char * Text;
  enum BaseTypes BaseType;
  struct IdList * IdList;
  struct LiteralDesc * LiteralDesc;
  struct InstrSeq * InstrSeq;
  struct ExprResult * ExprResult;
  struct CondResult * CondResult;
}

/* Type declaration for data attached to non-terminals. Allows     */
/* $# notation to be correctly type casted for function arguments. */
/* %type <union-field-name> non-terminal                           */
%type <Text> Id
%type <IdList> IdItem
%type <IdList> IdList
%type <BaseType> BaseType
%type <LiteralDesc> Literal
%type <BaseType> FuncDecl
%type <InstrSeq> CodeBlock
%type <InstrSeq> StmtSeq
%type <InstrSeq> Stmt
%type <InstrSeq> AssignStmt
%type <ExprResult> Expr
%type <ExprResult> Oprnd
%type <InstrSeq> IfStmt
%type <InstrSeq> WhileStmt
%type <CondResult> Cond
%type <InstrSeq> Else
%type <Text> Relation
%type <Text> String
%type <InstrSeq> Incr
%type <InstrSeq> Decr
%type <InstrSeq> ForLoop

/* List of token name, corresponding numbers will be generated */
/* y.tab.h will be generated from these */
/* %token symbolic-name text-used-in-rules */
%token IDENT_TOK
%token INT_TOK    	   "int"
%token CHR_TOK    	   "chr"
%token BOOL_TOK        "bool"
%token VOID_TOK        "void"
%token INTLIT_TOK
%token CHRLIT_TOK
%token STRLIT_TOK
%token BOOLLIT_TOK
%token ARROW_TOK       "->"
%token DBLCOLON_TOK    "::"
%token PUT_TOK         "put"
%token GET_TOK         "get"
%token FOR_TOK         "for"
%token IF_TOK          "if"
%token WHILE_TOK       "while"
%token ELSE_TOK        "else"
%token SWITCH_TOK      "switch"
%token BREAK_TOK       "break"
%token CASE_TOK        "case"

/* operator associativity and precedence in increasing order */
%left '+' '-'
%left '*' '/'
%left UMINUS

%%
// Step 1
Module        : DeclList                                    { };

DeclList      : Decl DeclList                               { };
DeclList      : Decl                                        { };

Decl          : IdList ':' BaseType                         { ProcDecl( $1, $3, 0); };
Decl          : IdList "::" Literal                         { ProcDecl( $1, $3->baseType, $3->value ); };
Decl          : IdList ':' FuncDecl                         { ProcDeclFunc( $1, $3 ); };
Decl          : IdList "::" CodeBlock                       { ProcFuncBody( $1, $3 ); };

IdList        : IdItem ',' IdList                           { $$ = AppendIdList( $1, $3 ); };
IdList        : IdItem                                      { $$ = $1; };

IdItem        : Id                                          { $$ = ProcName( $1, MAKE_SPAN( @1 ) ); };

Id            : IDENT_TOK                                   { @$ = @1; $$ = strdup( yytext ); };

BaseType      : INT_TOK                                     { @$ = @1; $$ = IntBaseType; };
BaseType      : CHR_TOK                                     { @$ = @1; $$ = ChrBaseType; };
BaseType      : BOOL_TOK                                    { @$ = @1; $$ = BoolBaseType; };
BaseType      : VOID_TOK                                    { @$ = @1; $$ = VoidBaseType; };

Literal       : INTLIT_TOK                                  { @$ = @1; $$ = MakeLiteralDesc( yytext, IntBaseType ); };
Literal       : CHRLIT_TOK                                  { @$ = @1; $$ = MakeLiteralDesc( yytext, ChrBaseType ); };
Literal       : BOOLLIT_TOK                                 { @$ = @1; $$ = MakeLiteralDesc( yytext, BoolBaseType ); };

FuncDecl      : '(' ')' "->" BaseType                       { $$ = VoidBaseType; };

CodeBlock     : '{' StmtSeq '}'                             { $$ = $2; };

StmtSeq       : Stmt StmtSeq                                { $$ = AppendSeq( $1, $2 ); };
StmtSeq       :                                             { $$ = NULL; };

// Step 2
Stmt          : AssignStmt                                  { $$ = $1; };
Stmt          : PUT_TOK '(' Expr ')'                        { $$ = ProcPut( $3 ); };

AssignStmt    : Id '=' Expr                                 { $$ = ProcAssign( $1, $3 ); };

Expr          : Expr '+' Expr                               { $$ = ProcExpr( $1, "add", $3 ); };
Expr          : Expr '-' Expr                               { $$ = ProcExpr( $1, "sub", $3 ); };
Expr          : Expr '*' Expr                               { $$ = ProcExpr( $1, "mul", $3 ); };
Expr          : Expr '/' Expr                               { $$ = ProcExpr( $1, "div", $3 ); };
Expr          : '(' Expr ')'                                { $$ = $2; };
Expr          : '-' Expr        %prec UMINUS                { $$ = ProcNegateExpr( $2 ); };
Expr          : GET_TOK '(' INT_TOK ')'                     { $$ = ProcGet(); };
Expr          : Oprnd                                       { $$ = $1; };

Oprnd         : Id                                          { $$ = LoadWord( $1 ); };
Oprnd         : INTLIT_TOK                                  { $$ = ProcLiteral( yytext, IntBaseType ); };
Oprnd         : CHRLIT_TOK                                  { $$ = ProcLiteral( yytext, ChrBaseType ); };
Oprnd         : BOOLLIT_TOK                                 { $$ = ProcLiteral( yytext, BoolBaseType ); };

// Step 3
Stmt          : IfStmt                                      { $$ = $1; };
Stmt          : WhileStmt                                   { $$ = $1; };

IfStmt        : IF_TOK Cond CodeBlock Else                  { $$ = ProcIfElse($2, $3, $4); };

WhileStmt     : WHILE_TOK Cond CodeBlock                    { $$ = ProcWhile( $2, $3 ); };

Cond          : Expr Relation Expr                          { $$ = ProcCond( $1, $2, $3 ); };

Else          : ELSE_TOK CodeBlock                          { $$ = $2; };
Else          :                                             { $$ = NULL; };

Relation      : '>' '='                                     { $$ = strdup( "blt" ); };
Relation      : '>'                                         { $$ = strdup( "ble" ); };
Relation      : '<' '='                                     { $$ = strdup( "bgt" ); };
Relation      : '<'                                         { $$ = strdup( "bge" ); };
Relation      : '#' '#'                                     { $$ = strdup( "beq" ); };
Relation      : '=' '='                                     { $$ = strdup( "bne" ); };

//Step 4
Stmt          : PUT_TOK '(' String  ')'                     { $$ = ProcPutString( $3 ); };
String        : STRLIT_TOK                                  { $$ = strdup( yytext ); };

//Step 5
Stmt          : Id '(' ')'                                  { $$ = CallFunc( $1 ); };

//Step 6
Stmt          : Incr                                                { $$ = $1; };
Stmt          : Decr                                                { $$ = $1; };
Incr          : Id '+' '+'                                          { $$ = ProcIncr( $1 ); };
Decr          : Id '-' '-'                                          { $$ = ProcDecr( $1 ); };

//Step 8
Stmt          : ForLoop                                             { $$ = $1; };
ForLoop       : FOR_TOK AssignStmt ',' Cond ',' Incr CodeBlock      { $$ = ProcForLoop( $2, $4, $6, $7 ); };
ForLoop       : FOR_TOK AssignStmt ',' Cond ',' Decr CodeBlock      { $$ = ProcForLoop( $2, $4, $6, $7 ); };

//Step 11

//Stmt          : Switch                                              { $$ = $1; };
//Switch        : SWITCH_TOK Id CaseList                              { $$ = ProcSwitch( $2, $3 ); };
//CaseList      : CaseItem BREAK_TOK CaseList                         { $$ = AppendCaseList( $1, $3 ); };
//CaseList      : CaseItem                                            { $$ = $1; };

//CaseItem      : CASE_TOK Literal CodeBlock                          { $$ = ProcCase( $2, $3 ); };

%%

void
yyerror( char *s )     {
  char * buf = malloc(strlen(s) + strlen(yytext) + 20 + 1);
  sprintf(buf,"ERROR: \"%s\" token: \"%s\"",s,yytext);
  PostMessage(LastTokenSpan(),buf);
  free(buf);
}
