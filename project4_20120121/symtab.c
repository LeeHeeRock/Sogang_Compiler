#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"
/*size of hash table*/
//#define SIZE 211
//#define SHIFT 4
int global_memloc = 0;
/* Procedure st_insert inserts the numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the first time, otherwise ignored*/
void var_insert(TreeNode * t, BucketList * s, int lineno, int* memloc, int flag)
{
	int h = hash(t->attr.name);
	BucketList l = s[h];
	/*Check whether the type of variable is VOID , IF so => error!*/
	if(t->child[0]->attr.type.in_type == Void){
//		printf("line : %d \"void %s\" ERROR, Type of variable cannot be VOID\n\n",t->lineno, t->attr.name );
//		exit(-1);
	}



	while((l != NULL) && (strcmp(t->attr.name,l->name)!= 0))
			l = l->next;
	if(l == NULL)/*variable not yet in table*/
	{
		l = (BucketList)malloc(sizeof(struct BucketListRec));
		l->name = t->attr.name;
		l->lines = (LineList)malloc(sizeof(struct LineListRec));
		l->lines->lineno = lineno;
		l->vpf = var;
	   	if(t->child[0]->attr.type.in_size > 0){
			l->isArray = 1;
			l->size = t->child[0]->attr.type.in_size;

			if(flag == 0){
				*memloc = *memloc - (4*(l->size));
				l->memloc = *memloc;
			}
			else if (flag == 1){
				
				l->memloc = 0;
				global_memloc = global_memloc + 4*(l->size);
				l->memloc = global_memloc;
			
			}
		}
		else{ 
			l->isArray = 0;	
		
			if(flag == 0){
				*memloc -= 4;
				l->memloc = *memloc;
			}
			else if (flag == 1){
				l->memloc = 0;
				global_memloc += 4;
				l->memloc = global_memloc;
			}
		}
		l->lines->next = NULL;
		l->next = s[h];
		s[h] = l;
	}	
	else
	{ 	/*TODO: var - var , var- func, var- param*/	
	 	printf("line %d : Duplicate declaration of '%s'\n\n", t->lineno, t->attr.name);
		exit(-1);
	}
}/*st_insert*/

void func_insert(TreeNode * t, FunctionList* s)
{
	TreeNode * temp;
	ParamList cur_param;

	int param_count = 0;
	int i;
	FunctionList l = *s;
	
	while((l) != NULL){
				
			if(!(strcmp( (l)->name, t->attr.name))){
				/*TODO : detect dupicated function name*/ 
				printf("line %d: ERROR, The name '%s' is already used for function\n\n", t->lineno, t->attr.name);
				exit(-1);			 
			}
			
			l = l->next;
	}
		
	if(l == NULL){
			//printf("d?\n");
		l = (FunctionList)malloc(sizeof(struct FunctionListRec));
		l->name = t->attr.name;
		
		l->lines = (LineList)malloc(sizeof(struct LineListRec));
		l->lines->lineno = t->lineno;
		/*TODO : how to handle memloc*/
		l->type = t->child[0]->attr.type.in_type;
		
		l->lines->next = NULL;
		l->next = (*s);
		(*s)=l;
		
		temp = t->child[1];

		if(temp->attr.type.in_size == -1){
		
			l->param_list = NULL;
		}
		else{
        l->param_list=(ParamList)malloc(sizeof(struct ParamListRec));
		cur_param = l->param_list;
			for(temp = t->child[1]; temp != NULL; temp = temp->sibling)
			{/*initialize the parameter of the function*/
			param_count +=1;
			//printf("param num : %d\n", param_count);
						
				if(temp->child[0]->attr.type.in_size == 1){
					
					cur_param->paramType = Array;
					cur_param->name = temp->attr.name;
					//printf("param : %s\n", cur_param->name);
				}else if((temp->child[0]->attr.type.in_size== 0)
						&& temp->child[0]->attr.type.in_type == Void){

					cur_param->paramType = Void;
					cur_param->name = temp->attr.name;	
					
				}else if( ( temp->child[0]->attr.type.in_size== 0)
						&& (temp->child[0]->attr.type.in_type == Int)){
					
					cur_param->paramType = Int;
					cur_param->name = temp->attr.name;
				}
			 cur_param->lineno = temp->lineno;
			 cur_param->next = (ParamList)malloc(sizeof(struct ParamListRec)); 
			 cur_param = cur_param->next;
			}
			free(cur_param);
			/*Set the Last parma list's next to NULL*/
			cur_param = (*s)->param_list;
			for(i=0; i < param_count-1 ; i++) cur_param = cur_param->next;	
			cur_param->next = NULL;
			/*set the number of parameters*/
			l->param_num = param_count;
												
		}
	
	}

}
/*Function st_lookup returns the memory
 *location of a variable or -1 if not found*/
int var_lookup(TreeNode * t, Scope ** s)
{
		int h = hash(t->attr.name);
		long int cur_scope;
	    cur_scope = *s;
		BucketList l;
		int line_to_store = t->lineno;
		do{
			l = (*s)->hashTable[h];
			while(l != NULL){
				if(!strcmp(l->name, t->attr.name)){
					LineList newLine;
					LineList search = l->lines;
					
					/* project4 store the memory location and check whether the variable is array or not
					   set the flag in the tree node! checked when calculate the memory offset in cgen code */
					t->memloc = l->memloc;
					t->is_array = l->isArray;
					t->global_var = (*s)->is_global;

					if(search->lineno == line_to_store){*s = cur_scope; return 1;}
					
					while(search->next != NULL){
						if(search->lineno == line_to_store){*s = cur_scope;	return 1;}
						search = search->next;
					}
					
					if(search->lineno == line_to_store){*s = cur_scope;	return 1;}
					
					newLine = (LineList)malloc(sizeof(struct LineListRec));
					newLine->lineno = t->lineno;
					search->next = newLine;
					newLine->next = NULL;

					*s = cur_scope;
					return 1; /*find*/
				}
			l = l->next;
			}
			*s = (*s)->prev;
		}while(*s != NULL);
		*s = cur_scope;
		return -1;	
}


int func_lookup(TreeNode * t, Scope **s)
{	/*TODO: handle param and return type sementic errors!...*/
	long int cur_scope;
	cur_scope = *s;
	int line_to_store = t->lineno;
   	FunctionList f;

	if(!strcmp(t->attr.name, "input") ||
			!strcmp(t->attr.name, "output"))
			return 1;

	while((*s)->is_global != 1){
	/*Move to global scope*/
		(*s) = (*s)->prev;
	}
	f = (*s)->functions;
	while(f != NULL){
		
		if(!strcmp(f->name, t->attr.name)){
			/*update*/
			LineList newLine;
			LineList search = f->lines;
			if(search->lineno == line_to_store){*s = cur_scope; return 1;}
					
					while(search->next != NULL){
						if(search->lineno == line_to_store){*s = cur_scope;	return 1;}
						search = search->next;
					}
					
					if(search->lineno == line_to_store){*s = cur_scope;	return 1;}
					
					newLine = (LineList)malloc(sizeof(struct LineListRec));
					newLine->lineno = t->lineno;
					search->next = newLine;
					newLine->next = NULL;

			*s = cur_scope;
			return 1; /*find*/
		}

		f = f->next;
	}

	*s = cur_scope;
	/*return -1 when success to reference*/
	return -1;
}
void printSymtab(FILE * listing, Scope *s)
{
int cnt = SIZE;
int i, j = 1;

BucketList l;
LineList q;
FunctionList f = s->functions;
fprintf(listing,"Name\tScope\tV/P/F\tArray?\tArrSize\tType\tLoc\tLine Numbers\n");
fprintf(listing,"-------------------------------------------------------------------------\n");

for(i = 0; i <SIZE ; i++){
	
	l = s->hashTable[i];
	
	if(l == NULL) continue;
	while(l != NULL){
	
			printf("%s\t%d\t",l->name, s->scope_number);
			if(l->vpf == var) 
				printf("var\t");
			else if(l->vpf == param) 
				printf("param\t");
			
			if(l->isArray == 1){
				printf("Yes\t%d\tarray\t",l->size);
			}
			else{
				printf("No\t");
		    	printf("-\tint\t");
			}
			
			printf("%d\t",l->memloc);
			
			for(q = l->lines; q !=NULL ; q = q->next){
				printf("%d ", q->lineno);
			}
			printf("\n");
	
	l = l->next;
	}
}

	while(f != NULL){
	/*print function information*/	
		printf("%s\t%d\t",f->name, s->scope_number);
		printf("Func\t");
		printf("No\t");
		printf("-\t");
		if(f->type == Int)
			printf("Int\t");
		else if(f->type == Void)
			printf("Void\t");
		
		printf("%d\t", j++);

		for(q = f->lines; q !=NULL ; q = q->next){
				printf("%d ", q->lineno);
		}
		f = f->next;
		printf("\n");
	}

printf("\n");
}


int var_func_name(TreeNode * t , Scope ** s){

	/*(1) goto global scope*/
	Scope * global = (*s);
	FunctionList f;
	int h = hash(t->attr.name);
	BucketList l;

	while(global->prev != NULL){
	
		global = global->prev;
	}	
	/*compared with previoulsy declared functions*/
	for(f = global->functions ; f != NULL; f = f->next){
		
		if(!strcmp(f->name, t->attr.name)){
		
			printf("line %d: ERROR, The name '%s' is already used for function\n\n", t->lineno, t->attr.name);
			return -1;
		}
	
	}


	return 1;
}

int func_var_name(TreeNode * t, Scope **s){
	
	Scope * global = (*s);
	FunctionList f;
	int h = hash(t->attr.name);
	BucketList l;
	
	while(global->prev != NULL){
	
		global = global->prev;
	}	
	
	l = global->hashTable[h];
	if(l == NULL) 
		return 1;
	/*compared with previously declared global variables*/
	while((l != NULL) && (strcmp(t->attr.name,l->name)!= 0)){
			printf("NAME : %s\n", l->name);
			l = l->next;
	}
	if(!strcmp(t->attr.name, l->name)&& l !=NULL){
			printf("line %d: ERROR, The name '%s' is already used for global variable\n\n", t->lineno, t->attr.name);
			return -1;
	}
}


int check_main(Scope * s){
	//paramter s is global scope pointer
	FunctionList f = s->functions;

	if((strcmp(f->name, "main"))!= 0 ){
	/*(1) Check whether the main is located at last of the code*/
		printf("line %d : Error, Main function must be declared at the last\n\n", f->lines->lineno);	
//		exit(-1);
		return -1;
	}

	if(f->type != Void){
	/*(2) Check whether return type of main function is void or not*/
		printf("line %d : Error, return type of main function must be void\n\n", f->lines->lineno);	
//		exit(-1);
		return -1;
	}

	if(f->param_list !=NULL){
		printf("line %d : Error, There can't be any parameter in main functions\n\n", f->lines->lineno);	
//		exit(-1);
		return -1;
	}
return 1;
}

int array_subs(TreeNode * t, Scope **s){

	int isArray = 0;
	int h = hash(t->attr.name);
	TreeNode * index = t->child[0];
	Scope * search  = (*s);
	BucketList l;	
	int ret;
	/*Check whether its type is array*/
	while(search != NULL){
	
		l = search->hashTable[h];
			
		while((l != NULL) && (strcmp(t->attr.name,l->name)!= 0))
			l = l->next;		
		
		if(l == NULL){ 
			search = search->prev; 
			continue;
		}	
		else{
			/*find it*/		
			if(l->isArray != 1){
				/*But not a array*/
				return 0;	
			}else{
				isArray = 1;
				break;
			}
		}
	search = search->prev;
	}	

	if(isArray == 0) return 0;

	/* NOW check the validity */
	ret = check_all_node_int(t->child[0], s);
	if(ret == -1){ 
			//printf("line %d : ERROR, '%s[]' Type(value) of Index of Array must be Int\n\n", t->lineno, t->attr.name); 
			//exit(-1);
			return -1;
	}
	return 1;

}

int check_all_node_int(TreeNode * t, Scope **s){
/*TODO make sure that there are no more cases to check!*/	
	TreeNode * start = t;
	int i;
	Scope * search = *(s);
	FunctionList f;
	
	//if(t->kind.exp == CallK) printf("adsfasdf\n");	
	while(search->prev != NULL){ search = search->prev; }

	f = search->functions;
    
	while(t != NULL){
		if(t->kind.exp == IdK){
			/*check whether type is int*/
			
		}else if(t->kind.exp == CallK){
			/*check whether return value is not void!*/
				while(f != NULL){
				
					if(!strcmp(f->name, t->attr.name)){
						if(f->type != Int){
						printf("line %d : Check the type of %s\n",t->lineno, t->attr.name);
			
						return -1;
						}
					}
					f = f->next;
				}
		
		}


		for(i=0; i<MAXCHILD; i++){
			if(check_all_node_int(t->child[i], s) == -1){
			 return -1;
			}
		}
		if(t == start) return 1;
		t = t->sibling;/*TODO handle this*/
	}

	return 1;
}

int used_like_array(TreeNode* t, Scope **s, int flag){
	//printf("line :%d, name : %s\n",t->lineno, t->attr.name);

		int h;
		Scope * search = (*s);
		BucketList l;	
		if(flag == 1) return 1;
	    /*case(1) : It is declared as Array But used without Array!\n*/	
		if(t->child[0] == NULL){
				h = hash(t->attr.name);
				//but what if used like non array variable..?
				while(search != NULL){
	
					l = search->hashTable[h];
			
						while((l != NULL) && (strcmp(t->attr.name,l->name)!= 0))
							l = l->next;		
		
							if(l == NULL){ 
								search = search->prev; 
								continue;
							}	
							else{
								/*find it*/		
								if(l->isArray == 1){
									printf("line %d : Check the type of %s\n",t->lineno,t->attr.name);
									/*But not a array*/
									return -2;	
								}else{ return 1; }
							}
						search = search->prev;
						}	
			   	return 1;
		}
		/*case(2) : It is declared as Int But used with Index subscript*/
		h = hash(t->attr.name);
		while(search != NULL){
	
			l = search->hashTable[h];
			
			while((l != NULL) && (strcmp(t->attr.name,l->name)!= 0))
				l = l->next;		
		
			if(l == NULL){ 
				search = search->prev; 
				continue;
			}	
			else{
				/*find it*/		
				if(l->isArray != 1){
					printf("line %d : Check the type of %s\n",t->lineno,t->attr.name);
					/*But not a array*/
					return -1;	
				}else{
					return 1;
				}
			}
		search = search->prev;
		}
		
}
int param_check(TreeNode *t, Scope ** s){

	int numOfArg = 0;
	TreeNode * child;
	Scope * search = *s;
	FunctionList f;
	ParamList p;
	char * name = t->attr.name;
	

	/*Project 4*/
	if(!strcmp(t->attr.name, "input") || !strcmp(t->attr.name, "output"))
			return 1;

	if(t->child[0] == NULL){
			numOfArg = 0;
	}
	else{
		for(child = t->child[0]; child !=NULL; child = child->sibling){
			numOfArg += 1;
		}
	}
	/*Step (1) : Find the function */		
	while(search->prev !=NULL) search = search->prev;
	
	for(f = search->functions; f->next !=NULL; f= f->next){		
		if(!strcmp(f->name, name)){ break;}	
	}
	
	if(f->param_num > numOfArg) 
				return -1;
	else if(f->param_num < numOfArg) 
				return -2;
	/*Step(3) : iter the param list and compare with argmetns passed*/
	p = f->param_list;
	
	if(p !=NULL){ 
		
		for(p = f->param_list, child = t->child[0]; p!=NULL || child !=NULL; p =p->next, child = child->sibling){
				//printf("param Type: %d, param name %s\n", p->paramType, p->name);
				
				if(p->paramType == Array){
				/*if the parmeter type is array then the argument type also be array*/
							char * name_to_find = child->attr.name;
						 	search = *s;
							int h = hash(name_to_find);
							BucketList l;
							do{
								l = search->hashTable[h];
								while(l != NULL){
									if(!strcmp(l->name, name_to_find)){
											if(l->isArray != 1){
											/*Type Error!*/
												printf("line %d : Check the type of \'%s\' in argument\n",child->lineno, name_to_find);
												return -3;
											}	
									}	
								l = l->next;
								}
							search = search->prev;
							}while(search != NULL);					
				}else if(p->paramType == Int){
				/*if the parmeter type is Int then the argument type also be Int!*/
				/*all the result and the type must be int!!!!*/
							//printf("%s\n", child->attr.name);
							int ret;
							if(child->kind.exp == OpK){
								/*check all the value is valid*/
									ret = check_all_node_int(child, s);
									if(ret == -1) return -4;
							
							}else if(child->kind.exp == CallK){
									while(f != NULL){
				
										if(!strcmp(f->name, child->attr.name)){
											if(f->type != Int){
											printf("line %d : Check the return type of %s(), void type function cannot be used!\n",child->lineno, child->attr.name);
											//exit(-1);
											return -4;
											}
										}
									f = f->next;
									}					
							}else if(child->kind.exp == IdK){
								/*Must not array type*/
								//(1) array with index, just int type variable	
								int isArray = 0;
								int h = hash(child->attr.name);
								TreeNode * index = child->child[0];
								Scope * search  = (*s);
								BucketList l;	
								/*Check whether its type is array*/
								while(search != NULL){
	
									l = search->hashTable[h];
			
									while((l != NULL) && (strcmp(child->attr.name,l->name)!= 0))
										l = l->next;		
										
										if(l == NULL){ 
											search = search->prev; 
											continue;
										}	
										else{
										/*find it*/
											
											if(l->isArray != 1){
										/*But not a array*/
													
											}else{
												isArray = 1;
												break;
											}
										}
								search = search->prev;
								}			
								if(isArray == 0) 
										return 1;												
								else{
									
									if(child->child[0]==NULL){
									printf("line %d: check the type of the argument %s, Array type variable with no subscript..\n", child->lineno, child->attr.name);
									return -4;			
									}				
								}			
							}
				}	
	
	}
	
	return 1;
	}
}

int check_ret_val(TreeNode *t, Scope ** s){

	FunctionList f;
	Scope * global;
	int ret;

	for(global = *s ; global->prev != NULL; global = global->prev);

	f = global->functions;
	
	if(f->type == Int){
			/*return value must be integer*/
			
			ret = check_all_node_int(t->child[0], s);		
			
			if(ret == -1){
				printf("Check the return type of function %s()\n",f->name);
				return -1;
			}
	}else if(f->type == Void){
			/*no return statement is allowed */
			return -2;
	
	}

}


int check_condition(TreeNode *t, Scope **s){
	
	int ret = 1;
	BucketList l;	
	if(t->child[0]->kind.exp == OpK){
			
		ret = check_all_node_int(t->child[0], s);
		
	}
	else if(t->kind.exp == IdK){
			
								/*Must not array type*/
								//(1) array with index, just int type variable	
								int isArray = 0;
								
								//printf("%s\n", t->child[0]->attr.name);
								int h = hash(t->child[0]->attr.name);
								
								TreeNode * index = t->child[0];
								Scope * search  = (*s);
								BucketList l;	
							
								/*Check whether its type is array*/
								while(search != NULL){
										
									l = search->hashTable[h];
			
									while((l != NULL) && (strcmp(t->child[0]->attr.name, l->name)!= 0))
										l = l->next;		
										
										if(l == NULL){ 
											search = search->prev; 
											continue;
										}	
										else{
										/*find it*/
											if(l->isArray != 1){
										/*But not a array*/
											}else{
												isArray = 1;
												break;
											}
										}
								search = search->prev;
								}			
								if(isArray == 0) 
										ret= 1;												
								else{
									if(t->child[0]==NULL){
									printf("line %d: check the type of the argument %s, Array type variable with no subscript..\n", t->lineno, t->attr.name);
									ret =  -1;			
									}				
								}			
	}else if(t->child[0]->kind.exp == CallK){
		FunctionList f;
		Scope * global;
	
		for(global = *s ; global->prev != NULL; global = global->prev);

		f = global->functions;
		while(f != NULL){
			if(!strcmp(f->name,t->child[0]->attr.name))break;
		
		}
		if(f->type != Int){
			/*return value must be integer*/
			printf("Check the return type of function %s(), only int type allowed...\n",f->name);
			ret = -1;
		}
	}	
	return ret;
}





void func_insert_two(TreeNode * t, FunctionList* s)
{
	TreeNode * temp;
	ParamList cur_param;

	int param_count = 0;
	int i;
	FunctionList l = *s;
	
	while((l) != NULL){
				
			if(!(strcmp( (l)->name, t->attr.name))){
				/*TODO : detect dupicated function name*/ 
				printf("line %d: ERROR, The name '%s' is already used for function\n\n", t->lineno, t->attr.name);
//				exit(-1);			 
			}
			
			l = l->next;
	}
		
	if(l == NULL){
			//printf("d?\n");
		l = (FunctionList)malloc(sizeof(struct FunctionListRec));
		l->name = t->attr.name;
		
		l->lines = (LineList)malloc(sizeof(struct LineListRec));
		l->lines->lineno = t->lineno;
		/*TODO : how to handle memloc*/
		l->type = t->child[0]->attr.type.in_type;
		
		l->lines->next = NULL;
		l->next = (*s);
		(*s)=l;
		
		temp = t->child[1];

		if(temp->attr.type.in_size == -1){
		
			l->param_list = NULL;
			l->param_num = 0;
		}
		else{
        l->param_list=(ParamList)malloc(sizeof(struct ParamListRec));
		cur_param = l->param_list;
			for(temp = t->child[1]; temp != NULL; temp = temp->sibling)
			{/*initialize the parameter of the function*/
			param_count +=1;
								
				if(temp->child[0]->attr.type.in_size == 1){
					
					cur_param->paramType = Array;
					cur_param->name = temp->attr.name;
				}else if((temp->child[0]->attr.type.in_size== 0)
						&& temp->child[0]->attr.type.in_type == Void){
					
					/*TODO : handle this as error!*/
					printf("line : %d \"void %s\" ERROR, Type of parameter cannot be VOID\n\n",temp->lineno, temp->attr.name );
					cur_param->paramType = Void;
					cur_param->name = temp->attr.name;	
					
				}else if( ( temp->child[0]->attr.type.in_size== 0)
						&& (temp->child[0]->attr.type.in_type == Int)){
					
					cur_param->paramType = Int;
					cur_param->name = temp->attr.name;
					
				}
			 cur_param->lineno = temp->lineno;
			 cur_param->next = (ParamList)malloc(sizeof(struct ParamListRec)); 
			 cur_param = cur_param->next;
			}
			free(cur_param);
			/*Set the Last parma list's next to NULL*/
			cur_param = (*s)->param_list;
			for(i=0; i < param_count-1 ; i++) cur_param = cur_param->next;	
			cur_param->next = NULL;
			/*set the number of parameters*/
			l->param_num = param_count;
		}
	}
}


void var_insert_two(TreeNode * t, BucketList * s, int lineno, int* memloc, int flag)
{
	int h = hash(t->attr.name);
	BucketList l = s[h];
	/*Check whether the type of variable is VOID , IF so => error!*/
	if(t->child[0]->attr.type.in_type == Void){
		printf("line : %d \"void %s\" ERROR, Type of variable cannot be VOID\n\n",t->lineno, t->attr.name);
	}



	while((l != NULL) && (strcmp(t->attr.name,l->name)!= 0))
			l = l->next;
	if(l == NULL)/*variable not yet in table*/
	{
		l = (BucketList)malloc(sizeof(struct BucketListRec));
		l->name = t->attr.name;
		l->lines = (LineList)malloc(sizeof(struct LineListRec));
		l->lines->lineno = lineno;
		l->vpf = var;
	   	if(t->child[0]->attr.type.in_size > 0){
			l->isArray = 1;
			l->size = t->child[0]->attr.type.in_size;
			
			/*Project 4 set the flag that this variable is array*/
			t->is_array = 1;
			
			if(flag == 0){
				*memloc = *memloc - (4*(l->size));
				l->memloc = *memloc;
				/*project4 store the memory location*/
				t->memloc = l->memloc;	
				t->global_var = 0;
			}
			else if (flag == 1){
				
				l->memloc = 0;
				global_memloc = global_memloc + 4*(l->size);
				l->memloc = global_memloc;
				/*project4 store the memory location*/
				t->memloc = l->memloc;
				t->global_var = 1;
			
			}
		}
		else{ 
			l->isArray = 0;	
			/*Project 4 set the flat that this variable is not array*/
			t->is_array = 0;
			if(flag == 0){
				*memloc -= 4;
				l->memloc = *memloc;
				/*project4 store the memory location*/
				t->memloc = l->memloc;
				t->global_var = 0;
			}
			else if (flag == 1){
				l->memloc = 0;
				global_memloc += 4;
				l->memloc = global_memloc;
				
				/*project4 store the memory location*/
				t->memloc = l->memloc;
				t->global_var = 1;
			}
		}
		l->lines->next = NULL;
		l->next = s[h];
		s[h] = l;
	}	
	else
	{ 	/*TODO: var - var , var- func, var- param*/	
	 	printf("line %d : Duplicate declaration of '%s'\n\n", t->lineno, t->attr.name);
		exit(-1);
	}
}/*st_insert*/
