#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#include "globals.h"
#define SIZE 211
#define SHIFT 4

typedef enum{var, param, func}VPF;

/*hash function*/
static int hash(char *key){

	int temp = 0;
	int i = 0 ;
	while (key[i]  != '\0'){
	 temp = ((temp << SHIFT) + key[i]) % SIZE;
	 ++i;
	}
	return temp;
}

/*the list of line numbers of the source 
 *code in which a variable is referenced
 */
typedef struct LineListRec
{
	  int lineno;
	  struct LineListRec * next;

} * LineList;

/*The record in the bucket lists for 
 *each variables, including name,
 *assigned memory location, and 
 *the list of line numbers in which 
 *it appears in the source code
 */

typedef struct BucketListRec
{
	  	
	  char * name; 		// name 
	  VPF vpf;			// variable/ parameter / function
	  int isArray;		// IF array : 1, otherwise : 0
	  int size;			// IF array : its size otherwise : -1
	  int scope;	  //TODO: determine whether check int bucket or in scope
	  LineList lines; 	// store line when this is used
	  int memloc; 		/*memory location for variable*/
	  
	  struct BucketListRec * next; // pointer to next 

} * BucketList;


typedef struct ParamListRec
{
	Types paramType;
	char * name;
	int lineno;
	struct ParamListRec * next;

}*ParamList;


typedef struct FunctionListRec
{
  	  char * name;
	  int param_num;
	  Types type;
	  ParamList param_list;	
	  LineList lines;
	  struct FunctionListRec * next;	  

}* FunctionList;

typedef struct __SCOPE
{ 	//this represent each scope
	char is_global;	
	
	/*Project 4*/
	char is_func_scope;
	char *func_name; 
	
	int scope_number;
	int param_mem_size;	
	BucketList hashTable[SIZE];
	FunctionList  functions;	 
	
	struct __SCOPE * next;
	struct __SCOPE * prev;

} Scope;


/*the hash table*/
//static BucketList hashTable[SIZE];

/* Procedure st_insert inserts the numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the first time, otherwise ignored*/

void var_insert(TreeNode * t, BucketList * s, int lineno, int *memloc, int flag);
void var_insert_two(TreeNode * t, BucketList * s, int lineno, int *memloc, int flag);

void func_insert(TreeNode * t, FunctionList * s);
void func_insert_two(TreeNode * t, FunctionList * s);


int var_lookup(TreeNode * t, Scope **s);
int func_lookup(TreeNode * t, Scope **s);
void printSymtab(FILE * listing, Scope *);
int var_func_name(TreeNode *t, Scope **s);
int func_var_name(TreeNode *t, Scope **s);
int check_main(Scope * s);
int array_subs(TreeNode *t, Scope **s);
int check_all_node_int(TreeNode * t, Scope ** s);
int used_like_array(TreeNode *t, Scope ** s, int flag);
int param_check(TreeNode *t, Scope **s);
int check_ret_val(TreeNode *t, Scope **s);
int check_condition(TreeNode *t, Scope **s);
void insert_IO(Scope **);
#endif
 
