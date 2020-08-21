#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/*counters for variable memory location*/
static int scope_depth = 0;
static int location = 0;
static char comp_after_function = 0;
static int id_in_call = 0;


void init_scope(Scope ** cur, FunctionList f, char *func_name){

	int i;
	if(comp_after_function == 2){
		comp_after_function = 0;
		return;
	}
	
	scope_depth++;
	
	(*cur)->next = (Scope* )malloc(sizeof(Scope));
	(*cur)->next->is_global = 0;
		
	(*cur)->next->prev = (*cur);
		
	(*cur) = (*cur)->next;
	(*cur)->is_func_scope = 0;
	(*cur)->scope_number = scope_depth;
	for(i =0; i<SIZE; i++){ (*cur)->hashTable[i] = NULL;}
	(*cur)->next = NULL;	
	if(comp_after_function == 1){
	/*After function declaration */
	/*need to process the parameters*/
		int cnt =0; 
		int i;
		FunctionList fl;
		ParamList pl;
		int memloc;		
		fl = f;
		cnt = fl->param_num;
		pl = fl->param_list;
		
		/*project 4*/
		(*cur)->is_func_scope = 1;
		(*cur)->func_name = func_name;

		/*all the parameter occupies 4 bytes*/
		memloc = cnt*4;
		
		/*project 4 : add param mem size to the scope*/
		(*cur)->param_mem_size = memloc;
		while(pl != NULL){
			
			int h = hash(pl->name);
			BucketList l = (*cur)->hashTable[h];
			
			//printf("memloc in init func : %d\n", memloc);
			
		    	while((l != NULL) && (strcmp(pl->name, l->name) != 0 ))
					l = l->next;
				if(l == NULL)
				{	
					l = (BucketList)malloc(sizeof(struct BucketListRec));
					l->name = pl->name;
					l->lines = (LineList)malloc(sizeof(struct LineListRec));
					l->lines->lineno = pl->lineno;
					l->vpf = param;
	   				if(pl->paramType == Array){
						l->isArray = 1;
						l->size = 0;
					}
					else 
						l->isArray = 0;	
		
					l->memloc = memloc;
					memloc -=4;
					/*TODO : how to handle memloc*/
					
					l->lines->next = NULL;
					l->next = (*cur)->hashTable[h];
					(*cur)->hashTable[h] = l;
				}	
			pl = pl->next;
		}
		
	comp_after_function = 2;
	}
}
void delete_scope(Scope ** cur){

	Scope *temp;

	temp = (*cur)->prev;

	free(*cur);

	*cur = temp;
	return;

}
static void traverse(TreeNode * t, Scope ** s,
					void (*preproc)(TreeNode *, Scope ** s, int * ml),
					void (*postproc)(TreeNode *))
{	
	int compound_flag = -1;
	static int local_memloc = 0;
	int temp_loc;
	if (t != NULL)
	{	
				
		if(t->thiskind == StmtK && t->kind.stmt == CompndK){
			compound_flag = 1;
			temp_loc = local_memloc;
		}
		preproc(t ,s, &local_memloc);
		{
		/*traverse its childs first*/
		 int i;
		 for(i = 0; i< MAXCHILD; i++)
				 traverse(t->child[i],s, preproc, postproc);
	    	
		}
		postproc(t);
		/*traverse its siblings*/
		if(t->thiskind == StmtK && t->kind.stmt == CompndK){
	 			local_memloc = temp_loc;
				/*Project 4*/
				scope_depth--;
		}
		traverse(t->sibling, s, preproc, postproc);
		//printSymtab(listing);
	}
	
	if(compound_flag == 1){
	 //printSymtab(listing, *s);
	 compound_flag = -1;
	 scope_depth--;
	 delete_scope(s);
	}

}
static void errorCheck(TreeNode * t, Scope ** s,
					void (*preproc)(TreeNode *, Scope ** s, int * ml),
					void (*postproc)(TreeNode *),
					runtime_stack **stack)
{
	int compound_flag = -1;
	static int local_memloc = 0;
	int temp_loc;
	/*project 4*/
	static int deepest_local_mem = -4;
	if (t != NULL)
	{
		if(t->thiskind == ExpK && t->kind.exp == CallK) id_in_call = 1;
		if(t->thiskind == StmtK && t->kind.stmt == CompndK){
	 		compound_flag = 1;
			temp_loc = local_memloc;
		}
		preproc(t ,s, &local_memloc);
		{
			
				
		/*traverse its childs first*/
		 int i;
		 for(i = 0; i<MAXCHILD; i++)
				 errorCheck(t->child[i],s, preproc, postproc, stack);
		}
		postproc(t);
		if(t->thiskind == StmtK && t->kind.stmt == CompndK){
	 			/*project 4*/
				if(local_memloc < deepest_local_mem){
					deepest_local_mem = local_memloc;
				}
				local_memloc = temp_loc;
				scope_depth--;
		}
		/*traverse its siblings*/
		errorCheck(t->sibling, s, preproc, postproc, stack);
		if(t->thiskind == ExpK && t->kind.exp == CallK && id_in_call == 1) id_in_call = 0;
		
	}
	if(compound_flag == 1){
	 compound_flag = -1;
	 scope_depth--;
	 struct stack * temp_stack;
	 int name_size;
	 		/*project 4*/
			if((*s)->is_func_scope == 1){
				/*can get a stack size!*/
				if((*stack) == NULL){
						(*stack) = (struct stack *)malloc(sizeof(struct stack));
						name_size = strlen((*s)->func_name);
						(*stack)->function_name = (char*)malloc(sizeof(char)*name_size + 1);
						strcpy((*stack)->function_name, (*s)->func_name);
						(*stack)->function_name[name_size+1] = '\0';
						(*stack)->next = NULL;
						(*stack)->prev = NULL;
						(*stack)->param_mem_size = (*s)->param_mem_size;
						(*stack)->local_var_mem_size = ((deepest_local_mem*(-1)) - 4);
						(*stack)->stack_size =  ((*stack)->local_var_mem_size) + (*stack)->param_mem_size + 8;
				}
				else{
						temp_stack = (struct stack *)malloc(sizeof(struct stack));
						temp_stack->next = NULL;
						temp_stack->prev = (*stack);
						(*stack)->next = temp_stack;
						(*stack) = (*stack)->next;
						name_size = strlen((*s)->func_name);
						(*stack)->function_name = (char*)malloc(sizeof(char)*name_size + 1);
						strcpy((*stack)->function_name, (*s)->func_name);
						(*stack)->function_name[name_size+1] = '\0';
						(*stack)->param_mem_size = (*s)->param_mem_size;
						(*stack)->local_var_mem_size = ((deepest_local_mem*(-1)) - 4);
						(*stack)->stack_size =  ((*stack)->local_var_mem_size) + (*stack)->param_mem_size + 8;
				}					
			deepest_local_mem = -4;
			}
	 delete_scope(s);
	}
}
static void nullproc(TreeNode * t)
{
	if(t == NULL) return;
	else return;

}

// t : AST  
// s : SymbolTable 
static void insertNode(TreeNode * t, Scope ** s, int* local)
{	
//	static id_in_call = 0;
	
	FunctionList dummy;
	char * dummy_name = NULL;
	int cnt;
	int global_flag;
	int ret;
	switch(t->thiskind)
	{
		int ret;
		case StmtK:
			switch(t->kind.stmt)
			{
			 case CompndK:
				/*need to make a new scope*/
				init_scope(s, dummy, dummy_name);
				break;
			 case SelectK:
				
				ret = check_condition(t,s);
				if (ret == -1){
						printf("line %d : ERROR, Only Integer value is allowed\n",t->lineno);	
				//		exit(-1);
				}
				break;
			 case IterK:
				
				ret = check_condition(t,s);
				if (ret == -1){
						printf("line %d : ERROR, Only Integer value is allowed\n",t->lineno);	
				//		exit(-1);
				}
				break;
			 case ReturnK:
			
				ret = check_ret_val(t, s);
				if(ret == -1){
					printf("line %d: ERROR, return value must be Int\n", t->lineno);
				//	exit(-1);
				}else if(ret == -2){
					printf("line %d: ERROR, No return statement is allowed\n", t->lineno);
				//	exit(-1);
				
				}
		   		break;		
			}
		break;

		case ExpK:
			switch(t->kind.exp)
			{
			 case OpK:
				
				/*Check whether all the children is int type */
				ret = check_all_node_int(t, s);
			   	if(ret == -1){
					printf("ERROR, Operand value must be Int type..\n\n");
				//	exit(-1);	
				}
				break;
			 case ConstK: 
				break;
			 case IdK:
				
			/*find its nearest declaration part add lineno*/
				ret = var_lookup(t, s);
				if(ret == -1){
					printf("lineno : %d\t ERROR : '%s' undeclared\n\n", t->lineno, t->attr.name);
				//	exit(-1);
				}
				//printf("Id in callll %d\n", id_in_call);
				ret = used_like_array(t,s, id_in_call);
				if(ret == -1){
					printf("lineno : %d\t ERROR : '%s' is not Array type, but used with subscript..\n\n", t->lineno, t->attr.name);
				//	exit(-1);	
				}else if(ret == -2){
					printf("lineno : %d\t ERROR : '%s' is Array type, but used without subscript..\n\n", t->lineno, t->attr.name);
				//	exit(-1);
				}
				ret = array_subs(t, s);
				if(ret == -1){
					printf("line %d : ERROR, '%s[]' Type(value) of Index of Array must be Int\n\n", t->lineno, t->attr.name);
				//	exit(-1);
				}
				break;
			 case TypeK: 
				break;
			 case ParamK: 
				break;
			 case CallK:
				
			/*find its nearest declaration part add lineno*/
				ret = func_lookup(t, s);
				if(ret == -1){
					printf("line %d: ERROR : Implicit declaration of function '%s'\n\n", t->lineno, t->attr.name);
				//	exit(-1);
				}
				ret = param_check(t,s);
				if(ret == -1){
					printf("line %d: ERROR : too few arguments to function '%s'\n\n", t->lineno, t->attr.name);
				//	exit(-1);
				}else if(ret == -2){
					printf("line  %d: ERROR : too many arguments to function '%s'\n\n", t->lineno, t->attr.name);
				//	exit(-1);
				}else if(ret == -3){
					printf("line %d: ERROR : wrong passing argument type  '%s'\n\n", t->lineno, t->attr.name);
				//	exit(-1);	
				}else if(ret == -4){
					printf("line %d: ERROR : argument type in function '%s'\n\n",t->lineno, t->attr.name);
				//	exit(-1);
				}
				break;
			}	
		break;	

		case DeclarK:
			
			switch(t->kind.declar)
			{
			 case VarDeclarK:
			 /*Add to the current scope table*/
			 	var_insert_two(t, (*s)->hashTable, t->lineno, local, (*s)->is_global);
			 /*Check with function name*/  	
				ret = var_func_name(t, s);
				if(ret == -1)exit(-1);		
			 break;
			 		 
			 case FunDeclarK:
			 	ret = func_var_name(t,s);
				if(ret == -1)exit(-1);
			 *local = -4;
			 /*Add to the current scope table*/
			 func_insert_two(t, &((*s)->functions));
			 /*make a scope and process the parameters*/
			 /*and then set the flag*/
			 comp_after_function = 1;
			 init_scope(s, (*s)->functions, t->attr.name);
			 break;
			}
		break;
	}

}

static void CreateNode(TreeNode * t, Scope ** s, int* local)
{
	FunctionList dummy;
	char * dummy_name = NULL;
	int cnt;
	int global_flag;
	int ret;
	switch(t->thiskind)
	{
		int ret;
		case StmtK:
			switch(t->kind.stmt)
			{
			 case CompndK:
				
				init_scope(s, dummy, dummy_name);
				break;
			 case SelectK:
				break;
			 case IterK:
				break;
			 case ReturnK:
				break;		
			}
		break;

		case ExpK:
			
			switch(t->kind.exp)
			{
			 case OpK:
			 	break;
			 case ConstK: 
				break;
			 case IdK:
			   		
			/*find its nearest declaration part add lineno*/
				ret = var_lookup(t, s);
				if(ret == -1){
					printf("lineno : %d\t ERROR : '%s' undeclared\n\n", t->lineno, t->attr.name);
					exit(-1);
				}
				break;
			 case TypeK: 
				break;
			 case ParamK: 
				break;
			 case CallK:
				
			/*find its nearest declaration part add lineno*/
				//printf("CALL %s\n", t->attr.name);
				ret = func_lookup(t, s);
				if(ret == -1){
					printf("line %d: ERROR : Implicit declaration of function '%s'\n\n", t->lineno, t->attr.name);
					exit(-1);
				}
				break;
			}	
		break;	

		case DeclarK:
			
			switch(t->kind.declar)
			{
			 case VarDeclarK:
			 /*Add to the current scope table*/
			 	var_insert(t, (*s)->hashTable, t->lineno, local, (*s)->is_global);
			 /*Check with function name*/  	
				ret = var_func_name(t, s);
				if(ret == -1)exit(-1);		
			 break;
			 		 
			 case FunDeclarK:
			
				ret = func_var_name(t,s);
				if(ret == -1)exit(-1);
			 	*local = -4;
			 	/*Add to the current scope table*/
			 	func_insert(t, &((*s)->functions));
			 	/*make a scope and process the parameters*/
			 	/*and then set the flag*/
			 	comp_after_function = 1;
			 	init_scope(s, (*s)->functions, t->attr.name);
			 break;
			}
		break;
	}

}

void buildSymtab(TreeNode * syntaxTree)
{
	int i;
	/*The function is called once make global first*/	
    /*init symtab*/
//	printf("build symtab!\n");
	Scope* SymTab = (Scope* )malloc(sizeof(Scope));		
 	SymTab->prev = NULL;
	SymTab->next = NULL;
	SymTab->is_global = 1;
	SymTab->functions = NULL;
	for(i =0; i<SIZE; i++) (SymTab)->hashTable[i] = NULL;	
	SymTab->scope_number = scope_depth;

	traverse(syntaxTree,&SymTab,CreateNode, nullproc);
 //	printSymtab(listing, SymTab);	
	free(SymTab);
}

void searchSymtab(TreeNode *syntaxTree, runtime_stack ** stack_instance)
{
	int i;
	/*The function is called once make global first*/	
    /*init symtab*/
	//printf("build symtab!\n");
	Scope* SymTab = (Scope* )malloc(sizeof(Scope));		
 	SymTab->prev = NULL;
	SymTab->next = NULL;
	SymTab->is_global = 1;
	SymTab->functions = NULL;
	for(i =0; i<SIZE; i++) (SymTab)->hashTable[i] = NULL;	
	SymTab->scope_number = scope_depth;

	errorCheck(syntaxTree,&SymTab,insertNode, nullproc, stack_instance);
 	
	check_main(SymTab);
	//printSymtab(listing, SymTab);	
	free(SymTab);
}
