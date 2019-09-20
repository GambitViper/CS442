/* Semantics.c
 Support and semantic action routines.

 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Scanner.h"
#include "YStructs.h"
#include "YSemantics.h"

// Shared Data

struct SymTab * IdentifierTable;
struct SymTab * StringLiteralTable;

// corresponds to enum Operators
char * Ops[] = { "add", "sub", "mul", "div"};

// enum Comparisons { LtCmp, LteCmp, GtCmp, GteCmp, EqCmp, NeqCmp };
// comparison set instructions, in order with enum Comparisons
char * CmpSetReg[] = { "slt", "sle", "sgt", "sge", "seq", "sne" };

void
InitSemantics() {
  IdentifierTable = CreateSymTab( 100, "global", NULL );
  StringLiteralTable = CreateSymTab( 100, "string", NULL );
}

bool
selectPrimType( struct SymEntry * entry ) {
  struct Attr * attr = GetAttr( entry );
  return attr->typeDesc->declType == PrimType;
}

void
processGlobalIdentifier( void * dataCode ) {
  struct SymEntry ** p = GetEntries( IdentifierTable, false, selectPrimType );
  while ( *p ) {
    const struct SymEntry * entry = *p;
    struct Attr * attr = GetAttr( entry );
    // gen code here
    char * value = malloc( sizeof( char ) * 25 );
    sprintf( value, "%d", attr->typeDesc->primDesc.initialValue );
    AppendSeq( dataCode, GenInstr( attr->reference, ".word", value, NULL, NULL, NULL ) );
    p++;
  }
}

void
processStringLiterals( void * dataCode ){
  struct SymEntry **p = GetEntries( StringLiteralTable, false, NULL );
  while ( *p ) {
    const struct SymEntry * entry = *p;
    struct Attr * attr = GetAttr( entry );
    AppendSeq( dataCode, GenInstr( attr->reference, ".asciiz", GetName( entry ), NULL, NULL, NULL ) );
    p++;
  }
}

bool
selectFuncType( struct SymEntry * entry ) {
  struct Attr * attr = GetAttr( entry );
  return attr->typeDesc->declType == FuncType;
}

void
processFunctions( void * textCode ) {
  // need to implement
  struct SymEntry ** p = GetEntries( IdentifierTable, false, selectFuncType );
  while ( *p ) {
    const struct SymEntry * entry = *p;
    struct Attr * attr = GetAttr( entry );
    AppendSeq( textCode, attr->typeDesc->funcDesc.funcCode );
    p++;
  }
}

// Semantics Actions

void
FinishSemantics() {
  // build text segment
  struct InstrSeq * textCode = GenOpX( ".text" );

  // form module preamble
  struct SymEntry * mainEntry = LookupName( IdentifierTable, "main" );
  CondPostMessageAndExit( !mainEntry, MakeEOFSpan(), "no main function for module" );

  // should make sure main takes no arguments
  struct Attr * mainAttr = GetAttr( mainEntry );
  printf( "%s\n", mainAttr->reference );

  // boilerplate for spim
  AppendSeq( textCode, GenOp1X( ".globl", "__start" ) );
  AppendSeq( textCode, GenLabelX( "__start" ) );

  AppendSeq( textCode, GenOp1X( "jal", mainAttr->reference ) );
  AppendSeq( textCode, GenOp2X( "li", "$v0", "10" ) );
  AppendSeq( textCode, GenOpX( "syscall" ) );

  // append code for all Functions
  processFunctions( textCode );

  // build data segment
  struct InstrSeq * dataCode = GenOpX( ".data" );
  processGlobalIdentifier( dataCode );
  processStringLiterals( dataCode );

  // combine and write
  struct InstrSeq * moduleCode = AppendSeq( textCode, dataCode );
  WriteSeq( moduleCode );

  // free code
  FreeSeq( moduleCode );
  CloseCodeGen();

  CloseSource();

  DisplaySymbolTable( IdentifierTable );
}

// Step 1

void
ProcDecl( struct IdList * idList, enum BaseTypes baseType, int initialValue ) {
  // walk idList item entries
  // get entry attributes
  // check required conditions
  // make and assign primitive type descriptor
  // make and assign id reference string
  while( idList ){
    struct SymEntry * entry = LookupName( IdentifierTable, GetName( idList->entry ) );
    struct Attr * attr = GetAttr( entry );
    attr->typeDesc = MakePrimDesc( baseType, initialValue );
    attr->reference = AppendStr( "_", GetName( entry ) );
    SetAttr( entry, STRUCT_KIND, attr );
    idList = idList->next;
  }
}

void
ProcDeclFunc( struct IdList * idList, enum BaseTypes type ) {
  // walk idList item entries
  // get entry attributes
  // check required conditions
  // make and assign function descriptor
  // make and assign id reference string
  while( idList ){
    struct SymEntry * entry = LookupName( IdentifierTable, GetName( idList->entry ) );
    struct Attr * attr = GetAttr( entry );
    attr->typeDesc = MakeFuncDesc( type );
    attr->reference = AppendStr( "_", GetName( entry ) );
    SetAttr( entry, STRUCT_KIND, attr );
    idList = idList->next;
  }
}

void
ProcFuncBody( struct IdList * idItem, struct InstrSeq * codeBlock ) {
  // check single item
  // check has typeDesc, if not create one indicating void return
  // wrap codeBlock in function entry label and jump return, assign to typeDesc
  struct SymEntry * entry = LookupName( IdentifierTable, GetName( idItem->entry ) );
  struct Attr * attr = GetAttr( entry );

  if( !attr->typeDesc ){
    attr->typeDesc = MakeFuncDesc( VoidBaseType );
  }
  attr->reference = AppendStr( "_", GetName( idItem->entry ) );
  SetAttr( entry, STRUCT_KIND, attr );

  struct InstrSeq * func_enter = GenLabelC( attr->reference, "func entry" );
  AppendSeq( func_enter, GenOp3X( "subu", "$sp", "$sp", "4" ) );
  AppendSeq( func_enter, GenOp2X( "sw", "$ra", "0($sp)" ) );

  struct InstrSeq * func_return = AppendSeq( func_enter, codeBlock );
  AppendSeq( func_return, GenLabel( AppendStr( GetName( idItem->entry ), "_ret" ) ) );
  AppendSeq( func_return, GenOp2X( "lw", "$ra", "0($sp)" ) );
  AppendSeq( func_return, GenOp3X( "addu", "$sp", "$sp", "4" ) );

  struct InstrSeq * jr = GenOp1C( "jr", "$ra", "func return" );

  attr->typeDesc->funcDesc.funcCode = AppendSeq( func_return, jr );

}

struct IdList *
AppendIdList( struct IdList * item, struct IdList * list ) {
  // chain item to list, return list
  struct IdList * temp = list;
  while( temp->next ){
    temp = temp->next;
  }
  temp->next = item;
  return list;
}

struct IdList *
ProcName( char * tokenText, struct Span span ) {
  // look up token text
  // check conditions
  // enter token text
  // create struct IdList * item
  // create and init attribute struct
  struct SymEntry * entry = LookupName( IdentifierTable, tokenText );
  if ( entry == NULL ){
    entry = EnterName( IdentifierTable, tokenText );
    struct Attr * attr = MakeAttr( NULL, NULL, span );
    SetAttr( entry, 0, attr );
  }
  struct IdList * idList = MakeIdList( entry, span );
  return idList;
}

struct InstrSeq *
ProcAssign( char * id, struct ExprResult * expr ){
  struct SymEntry * entry = LookupName( IdentifierTable, id );
  struct Attr * attr = GetAttr( entry );
  struct InstrSeq * assign = AppendSeq( expr->instr, GenOp2( "sw", TmpRegName( expr->reg ), attr->reference ) );
  // FreeExprResult( expr );
  return assign;
}

// Step 2
struct ExprResult *
ProcExpr( struct ExprResult * op1, char * op, struct ExprResult * op2 ){
  struct ExprResult * expr = MakeExprResult( IntBaseType );
  expr->reg = AvailTmpReg();
  expr->instr = AppendSeq( op1->instr, op2->instr );
  AppendSeq( expr->instr, GenOp3( op, TmpRegName( expr->reg ), TmpRegName( op1->reg ), TmpRegName( op2->reg ) ) );
  ReleaseTmpReg( op1->reg );
  ReleaseTmpReg( op2->reg );
  // FreeExprResult(op1);
  // FreeExprResult(op2);
  return expr;
}

struct ExprResult *
ProcNegateExpr( struct ExprResult * expr ){
  expr->reg = AvailTmpReg();
  expr->instr = GenOp2( "sub", "$zero", TmpRegName( expr->reg ) );
  return expr;
}

struct ExprResult *
ProcLiteral( char * text, enum BaseTypes type ){
  struct ExprResult * expr = MakeExprResult( type );
  expr->reg = AvailTmpReg();

  switch ( type ){
    case IntBaseType:
        expr->instr = GenOp2( "li", TmpRegName( expr->reg ), text);
        break;
    case ChrBaseType:
        if( '\\' == text[1] ){
          expr->instr = GenOp2( "li", TmpRegName( expr->reg ), Imm( 10 ) );
        }else {
          expr->instr = GenOp2( "li", TmpRegName( expr->reg ), Imm( text[1] ) );
        }
        break;
    case BoolBaseType:
        expr->instr = GenOp2( "li", TmpRegName( expr->reg ), Imm( ( strcmp( text, "true" ) ) ? 0 : 1 ) );
        break;
    case VoidBaseType:
        break;
  }

  return expr;
}

struct InstrSeq *
ProcPut( struct ExprResult * expr ){
  switch ( expr->type ) {
    case IntBaseType:
        AppendSeq( expr->instr, GenOp2( "li", "$v0", "1" ) );
        break;
    case BoolBaseType:
        // 'F' - value * ('T' - 'F')
        // li          $t0         0
        // li          $v0         11
        // mul         $t0         $t0         14               # PutExpr
        // add         $t0         $t0         70               # PutExpr
        // move        $a0         $t0
        AppendSeq( expr->instr, GenOp2( "li", "$v0", "11" ) );
        AppendSeq( expr->instr, GenOp3( "mul", TmpRegName( expr->reg ), TmpRegName( expr->reg ), "14" ) );
        AppendSeq( expr->instr, GenOp3( "add", TmpRegName( expr->reg ), TmpRegName( expr->reg ), "70" ) );
        break;
    case ChrBaseType:
        AppendSeq( expr->instr, GenOp2( "li", "$v0", "11" ) );
        break;
    case VoidBaseType:
        break;
  }

  AppendSeq( expr->instr, GenOp2( "move", "$a0", TmpRegName( expr->reg ) ) );
  struct InstrSeq * put = AppendSeq( expr->instr, GenOp( "syscall" ) );
  ReleaseTmpReg( expr->reg );
  // FreeExprResult( expr );
  return put;
}

struct ExprResult *
ProcGet(){
  struct ExprResult * expr = MakeExprResult( IntBaseType );
  expr->reg = AvailTmpReg();
  expr->instr = GenOp2C( "li", "$v0", "5", "read int syscall" );
  AppendSeq( expr->instr, GenOp( "syscall" ) );
  AppendSeq( expr->instr, GenOp2( "move", TmpRegName( expr->reg ), "$v0" ) );
  return expr;
}

struct ExprResult *
LoadWord( char * id ){
  struct SymEntry * entry = LookupName( IdentifierTable, id );
  struct Attr * attr = GetAttr( entry );
  struct ExprResult * expr = MakeExprResult( IntBaseType );
  expr->reg = AvailTmpReg();
  expr->instr = GenOp2("lw", TmpRegName( expr->reg ), attr->reference );
  return expr;
}

// Step 3
struct InstrSeq *
ProcIfElse( struct CondResult * cond, struct InstrSeq * ifbody, struct InstrSeq * elsebody ){
  if( !elsebody ){
    AppendSeq( cond->instr, ifbody );
    AppendSeq( cond->instr, GenLabelC( cond->label, "ProcIf" ) );
  }else {
    char * label = NewLabel();
    AppendSeq( cond->instr, ifbody);
    AppendSeq( cond->instr, GenOp1C( "b", label, "ProcIf" ) );
    AppendSeq( cond->instr, GenLabelC( cond->label, "ProcIf" ) );
    AppendSeq( cond->instr, elsebody );
    AppendSeq( cond->instr, GenLabelC( label, "ProcIf" ) );
  }
  struct InstrSeq * ifelse = cond->instr;
  // FreeCondResult( cond );
  return ifelse;
}

struct InstrSeq *
ProcWhile( struct CondResult * cond, struct InstrSeq * whilebody ){
  char * label = NewLabel();
  struct InstrSeq * winstr = GenLabelX( label );
  AppendSeq( winstr, cond->instr );
  AppendSeq( winstr, whilebody );
  AppendSeq( winstr, GenOp1X( "b", label ) );
  AppendSeq( winstr, GenLabelX( cond->label ) );
  // FreeCondResult( cond );
  return winstr;
}

struct CondResult *
ProcCond( struct ExprResult * left, char * rel, struct ExprResult * right ){
  struct CondResult * cond = MakeCondResult( NewLabel() );
  struct InstrSeq * relCode = AppendSeq( left->instr, right->instr );
  cond->instr = AppendSeq( relCode , GenOp3( rel, TmpRegName( left->reg ), TmpRegName( right->reg ), cond->label ) );
  ReleaseTmpReg( left->reg );
  ReleaseTmpReg( right->reg );
  // FreeExprResult( left );
  // FreeExprResult( right );
  return cond;
}

//Step 4
struct InstrSeq *
ProcPutString( char * str ){
  // struct SymEntry * entry = LookupName( IdentifierTable, tokenText );
  // if ( entry == NULL ){
  //   entry = EnterName( IdentifierTable, tokenText );
  //   struct Attr * attr = MakeAttr( NULL, NULL, span );
  //   SetAttr( entry, 0, attr );
  // }
  // struct IdList * idList = MakeIdList( entry, span );
  // return idList;
  struct SymEntry * entry = LookupName( StringLiteralTable, str );
  struct Attr * attr = NULL;
  if ( entry == NULL ){
    entry = EnterName( StringLiteralTable, str );
    attr = MakeAttr( NULL, NewLabel(), MakeSpan(-1, -1) ); //hacky fix for span error
    SetAttr( entry, 0, attr );
  }
  attr = GetAttr( entry );
  struct InstrSeq * instr = GenOp2( "li", "$v0", "4" );
  AppendSeq( instr, GenOp2( "la", "$a0", attr->reference ) );
  AppendSeq( instr, GenOp( "syscall") );
  return instr;
}

//Step 5
struct InstrSeq *
CallFunc( char * id ){
  return GenOp1X( "jal", AppendStr( "_", id ) );
}

//Step 6
struct InstrSeq *
ProcIncr( char * id ){
  return ProcAssign( id, ProcExpr( LoadWord( id ), "add", ProcLiteral( "1", IntBaseType ) ) );
}

struct InstrSeq *
ProcDecr( char * id ){
  return ProcAssign( id, ProcExpr( LoadWord( id ), "sub", ProcLiteral( "1", IntBaseType ) ) );
}

//Step 8
struct InstrSeq *
ProcForLoop(struct InstrSeq * assign, struct CondResult * cond, struct InstrSeq * by, struct InstrSeq * body ){
  char * label = NewLabel();
  struct InstrSeq * loop = AppendSeq( assign, GenLabelX( label ) );
  AppendSeq( loop, cond->instr );
  AppendSeq( loop, body );
  AppendSeq( loop, by );
  AppendSeq( loop, GenOp1X( "b", label ) );
  AppendSeq( loop, GenLabelX( cond->label ) );
  return loop;
}

//Step 11
// struct ProcCase( $2, $3 )
