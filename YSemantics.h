/* Semantics.h
 The action and supporting routines for performing semantics processing.
 */

#include "SymTab.h"
#include "IOMngr.h"
#include "YCodeGen.h"
#include "YStructs.h"

// Supporting Routines

void InitSemantics();
void FinishSemantics();

// Semantics Actions

// Step 1
void                    ProcDecl(struct IdList * idList, enum BaseTypes baseType, int initialValue);
void                    ProcDeclFunc(struct IdList * idList, enum BaseTypes type);
void                    ProcFuncBody(struct IdList * idItem, struct InstrSeq * codeBlock);
struct IdList *         AppendIdList(struct IdList * item, struct IdList * list);
struct IdList *         ProcName(char * tokenText, struct Span span);

//Step 2
struct InstrSeq *       ProcAssign(char * id, struct ExprResult * expr);
struct ExprResult *     ProcExpr(struct ExprResult * op1, char * op, struct ExprResult * op2);
struct ExprResult *     ProcNegateExpr(struct ExprResult * expr);
struct ExprResult *     ProcLiteral(char * text, enum BaseTypes type);
struct InstrSeq *       ProcPut(struct ExprResult * expr);
struct ExprResult *     ProcGet();
struct ExprResult *     LoadWord(char * id);

//Step 3
struct InstrSeq *       ProcIfElse(struct CondResult * cond, struct InstrSeq * ifbody, struct InstrSeq * elsebody);
struct InstrSeq *       ProcWhile(struct CondResult * cond, struct InstrSeq * whilebody);
struct CondResult *     ProcCond(struct ExprResult * left, char * rel, struct ExprResult * right);

//Step 4
struct InstrSeq * ProcPutString(char * str);

//Step 5
struct InstrSeq * CallFunc(char * id);

//Step 6
struct InstrSeq * ProcIncr(char * id);
struct InstrSeq * ProcDecr(char * id);

//Step 8
struct InstrSeq * ProcForLoop(struct InstrSeq * assign, struct CondResult * cond, struct InstrSeq * by, struct InstrSeq * body);
