#ifndef _ANALYZE_H
#define _ANALYZE_H


/*Function buiSymtab constructs the symbol
 *table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode *);

void typecheck(TreeNode *);

void searchSymtab(TreeNode *);
#endif
