#include "globals.h"
#include "util.h"


void printToken(TokenType token, const char* tokenString){

	int len, i;
	/*TODO - correct and hardcoding for eeach tokens*/
	
	//if(tokenString == EOF) fprintf(listing,"\t\tEOF\t\t\n"); return;
	switch (token)
	{	
		
		case IF: 
			fprintf(listing, "\t\t%s\t\t%s\n","IF", tokenString);
			break;
		case ELSE:
			fprintf(listing, "\t\t%s\t\t%s\n","ELSE", tokenString);
			break;
		case INT: 
			fprintf(listing, "\t\t%s\t\t%s\n","INT", tokenString);
			break;
		case RETURN:
			fprintf(listing, "\t\t%s\t\t%s\n","RETURN", tokenString);
			break;
		case VOID: 
			fprintf(listing, "\t\t%s\t\t%s\n","VOID", tokenString);
			break;
		case WHILE:
			fprintf(listing, "\t\t%s\t\t%s\n","WHILE", tokenString);
			break;
		case PLUS:  
			fprintf(listing, "\t\t%s\t\t%s\n","+", tokenString);
			break;
		case MINUS:
			fprintf(listing, "\t\t%s\t\t%s\n","-", tokenString);
			break;
		case TIMES: 
			fprintf(listing, "\t\t%s\t\t%s\n","*", tokenString);
			break;
		case DIVIDE:
			fprintf(listing, "\t\t%s\t\t%s\n","/", tokenString);
			break;
		case LT: 
			fprintf(listing, "\t\t%s\t\t%s\n","<", tokenString);
			break;
		case LTE:
			fprintf(listing, "\t\t%s\t\t%s\n","<=", tokenString);
			break;
		case GT: 
			fprintf(listing, "\t\t%s\t\t%s\n",">", tokenString);
			break;
		case GTE:
			fprintf(listing, "\t\t%s\t\t%s\n",">=", tokenString);
			break;
		case EQ: 
			fprintf(listing, "\t\t%s\t\t%s\n","==", tokenString);
			break;
		case NEQ:
			fprintf(listing, "\t\t%s\t\t%s\n","!=", tokenString);
			break;
		case ASSIGN: 
			fprintf(listing, "\t\t%s\t\t%s\n","=", tokenString);
			break;
		case SEMI:
			fprintf(listing, "\t\t%s\t\t%s\n",";", tokenString);
			break;
		case COM: 
			fprintf(listing, "\t\t%s\t\t%s\n",",", tokenString);
			break;
		case LPAREN:
			fprintf(listing, "\t\t%s\t\t%s\n","(", tokenString);
			break;
		case RPAREN: 
			fprintf(listing, "\t\t%s\t\t%s\n",")", tokenString);
			break;
		case LSQBR:
			fprintf(listing, "\t\t%s\t\t%s\n","[", tokenString);
			break;
		case RSQBR: 
			fprintf(listing, "\t\t%s\t\t%s\n","]", tokenString);
			break;
		case LCRBR:
			fprintf(listing, "\t\t%s\t\t%s\n","{", tokenString);
			break;
		case RCRBR:
			fprintf(listing, "\t\t%s\t\t%s\n","}", tokenString);
			break;	
		case ID:
			fprintf(listing, "\t\tID\t\t%s\n",tokenString); break;	
		case NUM:
			fprintf(listing, "\t\tNUM\t\t%s\n",tokenString); break;		
		case ENDFILE:
			fprintf(listing,"\t\tEOF\t\t\n"); break;
		case CMTERR:
			fprintf(listing,"\t\tERROR\t\tComment Error\n");break;
		default:
			fprintf(listing, "\t\tERROR\t\t%s\n",tokenString);
	}
}

TreeNode * newStmtNode(StmtKind kind)
{
	 //printf("Stmt node making\n");	 	
	 TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
 	 int i;
 	 if (t == NULL)
		 fprintf(listing, "Out of memory error at line %d\n", lineno);
 	 else {
 
 			for (i=0; i<MAXCHILD; i++) t -> child[i] = NULL;

				t->sibling = NULL;
				t->thiskind = StmtK;
				t->kind.stmt = kind;
				t->lineno = lineno;
 	}
	
	return t;
}

TreeNode * newExpNode(ExpKind kind)
{

	//printf("Exp node making\n");	
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if(t == NULL)
			fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		    for(i=0; i<MAXCHILD; i++) t->child[i] = NULL;
			t->sibling = NULL;
			t->thiskind = ExpK;
			t->kind.exp = kind;
			t->lineno = lineno;	
	}
	
	return t;
}

TreeNode * newDeclarNode(DeclarKind kind)
{
	
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if(t == NULL)
			fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		    for(i=0; i<MAXCHILD; i++) t->child[i] = NULL;
			t->sibling = NULL;
			t->thiskind = DeclarK;
			t->kind.declar = kind;
			t->lineno = lineno;
	}
	
	return t;
}


char * copyString(char * s)
{
 //printf("copy call, copied string: %s\n", s);
 int n;
 char * t;
 if (s == NULL) return NULL;
 n = strlen(s) + 1;
 t = malloc(n);
 if (t == NULL)
	fprintf(listing, "Out of memory error at line %d\n", lineno);
 else strcpy(t, s);
 return t;
}

static int indentno = 2;

#define INDENT indentno+=2
#define UNINDENT indentno-=2

static void printSpace(void){

	int i;
	for( i= 0; i<indentno; i++)
			fprintf(listing, " ");
}
void printOp(int op){
	
	switch(op){
	
			case GT: 
					fprintf(listing, ">\n");
					break;
			case LT: 
					fprintf(listing, "<\n");
					break;
			case GTE: 
					fprintf(listing, ">=\n");
					break;
			case LTE: 
					fprintf(listing, "<=\n");
					break;
			case EQ: 
					fprintf(listing, "==\n");
					break;
			case NEQ: 
					fprintf(listing, "!=\n");
					break;
			case PLUS:
					fprintf(listing, "+\n");
					break;
			case MINUS:
					fprintf(listing, "-\n");
					break;
			case TIMES:
					fprintf(listing, "*\n");
					break;
			case DIVIDE:
					fprintf(listing, "/\n");
					break;
			case ASSIGN:
					fprintf(listing, "=\n");
					break;
	}

}
;
void printType(struct Type type){
/*TODO : Unite the expression of Array type*/
	fprintf(listing, "Type : ");
    if(type.in_type == Int && type.in_size == 0)
			fprintf(listing, "Int\n");
	else if(type.in_type == Int && type.in_size == 1)
			fprintf(listing, "Int Array\n");
	else if (type.in_type == Void)
			fprintf(listing, "Void\n");
	else if (type.in_type == Array && type.in_size > 1){
			fprintf(listing, "Int\n");
			printSpace();
			fprintf(listing, "Const: %d\n",type.in_size);
	}
}
void get_Name(TreeNode *A, TreeNode *B){
		
		int len;
		
		len = strlen(B->attr.name);
		A->attr.name = (char *)malloc(sizeof(char)*(len+1));
		strcpy(A->attr.name,B->attr.name);
		A->lineno = B->lineno;
		free(B);
}
void get_Size(TreeNode *A, TreeNode *B){
		
		A->child[0]->attr.type.in_type = Array;
		A->child[0]->attr.type.in_size = B->attr.val;
		free(B);
}
/*print TREE~~*/
void printTree (TreeNode * tree){
	
	int i;
	TreeNode * temp;

	while (tree != NULL ){
		printSpace();
		/*case of Declaration*/
		if (tree->thiskind == DeclarK){
			switch (tree->kind.declar){
				case VarDeclarK :
					    	
						fprintf(listing, "ID : %s\n", tree->attr.name);
						break;
				case FunDeclarK : 
						fprintf(listing, "Function : %s\n", tree->attr.name);
						
						break;
			}
		}
		else if(tree->thiskind == StmtK){
			
			switch (tree->kind.stmt){
				 case CompndK :
						fprintf(listing, "Compound statement\n");
						break;
			
				 case SelectK :
						fprintf(listing, "If\n");
						break;
				 case IterK :
						fprintf(listing, "While\n");
						break;
				 case ReturnK :
						fprintf(listing, "Return\n");
						break;
			}
		
		
		}
		/*case of Expression*/
		else if (tree->thiskind == ExpK){
		
			switch (tree->kind.exp){
			
					case TypeK : 
						printType(tree->attr.type);
						break;
					case ParamK :
					    if(tree->attr.type.in_type==Void&&tree->attr.type.in_size==-1)
					   	  	fprintf(listing,"Parameter : (VOID)\n");
					    else
							fprintf(listing,"Parameter : %s\n", tree->attr.name);
						break;
					case OpK :
						fprintf(listing, "Op : ");
						printOp(tree->attr.op);
						break;
					case IdK :
						fprintf(listing, "ID : %s\n", tree->attr.name);
						break;
					case ConstK :
						fprintf(listing, "Const : %d\n", tree->attr.val);
						break;
					case CallK :
						fprintf(listing, "Call Procedure : %s\n", tree->attr.name);
					    break;
						
			}
		}
		else fprintf(listing, "Unknwon node kind\n");
		INDENT;
		for (i =0; i<MAXCHILD; i++){
			printTree(tree->child[i]);
		}
		UNINDENT;
		tree = tree->sibling;
		
	}
}

