#ifndef _CGEN_H_
#define _CFEN_H_
#include "globals.h"
#include "symtab.h"

void codeGen(TreeNode *, FILE *, runtime_stack *);
void genStmt(TreeNode *, FILE *, runtime_stack *);
int genExpK(TreeNode *, FILE *,runtime_stack * ,int);
void genDeclarK(TreeNode *, FILE *, runtime_stack *);
void addIO(FILE *);
void DeallocStack(FILE *,runtime_stack *);

#endif 
