#ifndef _UTIL_H_
#define _UTIL_H_

#define symb_dis 19


void printToken( TokenType, const char *);

TreeNode * newStmtNode(StmtKind);
TreeNode * newDeclarNode(DeclarKind);
TreeNode * newExpNode (ExpKind);

char * copyString (char *);

void printTree(TreeNode *);
void get_Size(TreeNode *, TreeNode *);
void get_Name(TreeNode *, TreeNode *);
#endif
