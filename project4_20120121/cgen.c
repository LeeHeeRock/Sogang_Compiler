#include"cgen.h"
#include"globals.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#define left 0
#define right 1
int used_reg[13] = {0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}; 
char* reg_name[13] = {"$t0", "$t1", "$t2", "$t3","$t4","$t5","$t6","$t7",
							 "$s4", "$s5", "$s6", "$s7","$v0"};
int used_arg[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
char* arg_name[8] = {"$a0", "$a1", "$a2", "$a3","$s0","$s1","$s2","$s3"};


char function_flag = 0;
char * cur_function;
/****************
 help function
*****************/
int get_reg(){

	int i;
	for(i = 0; i<12; i++){
		if(used_reg[i] == 0){
				used_reg[i] = 1;
				return i;
		}
	}

	printf("all the regs are occupied now!\n");
}
int get_arg_reg(){

	int i;
	for(i = 0; i<8; i++){
		if(used_arg[i] == 0){
				used_arg[i] = 1;
				return i;
		}
	}
}
int gen_label(int flag){

 	static int num = 0;
	if(flag == 1){
		num += 1;
	}	
	else{
		num -= 1;
	}

	return num;
}

/*traverse the AST and create the assemble code*/
void codeGen(TreeNode * AST, FILE * codefile, runtime_stack * stack_instance){

	int l_reg;
	int r_reg;
	int unused_reg;
	int index_val_reg;
	
	
	int array_val_reg;
	int array_base_reg;
	int offset_reg;
	int array_find_label;
	int array_not_find_label;
	int cur_fp_addr;
	
	if(AST != NULL)
	{
		switch(AST->thiskind){
			
			case StmtK:
//			printf("D1\n");							
				if(AST->kind.stmt == CompndK) 
						function_flag +=1;
			   	else if(AST->kind.stmt == ReturnK){
			
					r_reg = genExpK(AST->child[0], codefile, stack_instance, right);
					fprintf(codefile,"\tmove $v0 %s\n", reg_name[r_reg]);
					used_reg[r_reg] = 0;
				}else{
					genStmt(AST, codefile,stack_instance);
				}

				break;

			case ExpK:
//			printf("D2\n");
				/*need to traverse bottom up and left child first*/
				if(AST->kind.exp == OpK && AST->attr.op == ASSIGN){
					
					r_reg =	genExpK(AST->child[1], codefile, stack_instance,right);
				    	
					
					if(!AST->child[0]->is_array){
//					printf("D3\n");	
						if(!AST->child[0]->global_var)
							fprintf(codefile,"\tsw %s, %d($fp)\n", reg_name[r_reg], AST->child[0]->memloc);
						else{
							unused_reg = get_reg();
							fprintf(codefile,"\taddu %s, $gp, %d\n", reg_name[unused_reg], AST->child[0]->memloc);
							fprintf(codefile,"\tsw %s, 0(%s)\n", reg_name[r_reg], reg_name[unused_reg]);
							used_reg[unused_reg] = 0;	
						}
					}
					else{
//					printf("D4\n");
						unused_reg = get_reg();
						if(AST->child[0]->global_var == 0){//case of local array variable
							
							
								if(AST->child[0]->memloc > 0){
//								printf("D5\n");		
									//and the array is passed as parameter 	
									//step 1) find the base addresss of the array -> using fp and its offset stored in the parameter stack
									used_reg[unused_reg] = 0;
									array_base_reg = get_reg();
									offset_reg = get_reg();
									array_find_label = gen_label(1);
									array_not_find_label = gen_label(1);
									fprintf(codefile, "\tlw %s, 0($fp)\n", reg_name[array_base_reg]);
									fprintf(codefile, "\taddi %s, $fp, %d\n", reg_name[offset_reg], AST->child[0]->memloc);
					
									//if not found, then branch to here!
									fprintf(codefile, "L%d:\n", array_not_find_label);
					
									fprintf(codefile, "\tlw %s, 0(%s)\n", reg_name[offset_reg], reg_name[offset_reg]);
									//fprintf(file, "\n\tmove $a0, %s\n\tli $v0, 1\n\tsyscall\n",reg_name[offset_reg]);	

									fprintf(codefile, "\tbltz %s, L%d\n",reg_name[offset_reg], array_find_label);
									fprintf(codefile, "\taddu %s, %s, %s\n", reg_name[offset_reg], reg_name[array_base_reg], reg_name[offset_reg]);
									fprintf(codefile, "\tlw %s, 0(%s)\n", reg_name[array_base_reg], reg_name[array_base_reg]);
									fprintf(codefile, "\tb L%d\n", array_not_find_label);
										//if found, then branch to here!
									fprintf(codefile, "L%d:\n", array_find_label);
									//if not works, then change add -> abs and subu
									fprintf(codefile, "\tneg %s, %s\n", reg_name[offset_reg], reg_name[offset_reg]);
									fprintf(codefile, "\tsubu %s, %s, %s\n",reg_name[array_base_reg], reg_name[array_base_reg], reg_name[offset_reg]);
									//deallocate the regs 
									used_reg[offset_reg] = 0;
									/*if(AST->child[0]->child[0] == NULL)
								 		printf("it is null\n");
									else
										printf("it is not null!\n");	*/
									index_val_reg = genExpK(AST->child[0]->child[0],codefile,stack_instance ,left);
	    							fprintf(codefile, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
  								    fprintf(codefile, "\taddu %s, %s, %s\n",reg_name[array_base_reg],reg_name[array_base_reg], reg_name[index_val_reg]);
 								    used_reg[index_val_reg] = 0;
				 				   	fprintf(codefile, "\tsw %s, 0(%s)\n", reg_name[r_reg], reg_name[array_base_reg]);
									used_reg[array_base_reg] = 0;
									//used_reg[array_val_reg] = 0;	
								}
								else{	
//								printf("D6\n");
									//step 1 : store the base address temporarily to randomly selected register
									fprintf(codefile,"\tsubu %s, $fp, %d\n",reg_name[unused_reg], (AST->child[0]->memloc)*-1);
									//step 2 : get the register that stores a value of expression 	
			   						index_val_reg = genExpK(AST->child[0]->child[0],codefile, stack_instance,left);
									//step 3 : multiply 4 the value stored in the register
									fprintf(codefile, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
									//step 4 : move the register storing the base address of the array
									fprintf(codefile, "\taddu %s, %s, %s\n",reg_name[unused_reg],reg_name[unused_reg], reg_name[index_val_reg]);
									used_reg[index_val_reg] = 0;
									fprintf(codefile, "\tsw %s, 0(%s)\n", reg_name[r_reg], reg_name[unused_reg]);
								}
						}
						else if(AST->child[0]->global_var == 1){//case of global array variable
//							printf("D7\n");
							//step 1 : store the base address temporarily to randomly selected register
							fprintf(codefile,"\taddu %s, $gp, %d\n",reg_name[unused_reg], ((AST->child[0]->memloc)));
							//setp 2 : get the register that stores a value of expression 	
			   				index_val_reg = genExpK(AST->child[0]->child[0], codefile,stack_instance, left);
							//step 3 : multiply 4 the value stored in the register
							fprintf(codefile, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
							//step 4 : move the register storing the base address of the array
						fprintf(codefile, "\tsubu %s, %s, %s\n",reg_name[unused_reg],reg_name[unused_reg], reg_name[index_val_reg]);
							used_reg[index_val_reg] = 0;
							fprintf(codefile,"\tsw %s, 0(%s)\n", reg_name[r_reg],reg_name[unused_reg]);
						}			
					}				
					used_reg[r_reg] = 0;
				 }else if(AST->kind.exp == CallK){
//						printf("D8\n");
						  int reg;
							

						  if(!strcmp(AST->attr.name, "output")){
//	    					printf("D9\n");
//							printf("name : %s, num : %d\n", AST->attr.name, AST->child[0]->global_var);
							//need to get memory location of variable to print_out
				 			fprintf(codefile, "\n# output\n");
				 			fprintf(codefile, "\tli $v0, 4\n");
				 			fprintf(codefile, "\tla $a0, output_str\n");
				 			fprintf(codefile, "\tsyscall\n");
				 			
							//fprintf(codefile, "\tli $v0, 1\n");
				 			reg = genExpK(AST->child[0], codefile,stack_instance ,right);
//							printf("reg num : %d\n",reg);
							if(!(AST->child[0]->global_var)){
				 				
								fprintf(codefile, "\tmove $a0, %s\n",reg_name[reg]);
							}
							else{
//								printf("is here?\n");
								if(AST->child[0]->is_array)
									fprintf(codefile, "\tmove $a0, %s\n",reg_name[reg]);
								else
									fprintf(codefile, "\tmove $a0, %s\n",reg_name[reg]);
							}
				 			
							used_reg[reg] = 0;
							
							fprintf(codefile, "\tli $v0, 1\n");
				 			fprintf(codefile, "\tsyscall\n");
							//need to print newline character
							fprintf(codefile, "\tli $v0, 4\n");
							fprintf(codefile, "\tla $a0, newline\n");
							fprintf(codefile, "\tsyscall\n");
						}else{
//							printf("D10\n");
							//case of calling user-defined procedure
							int numOfarg;
							/*step 1 : find stack_instance*/
							char * label = AST->attr.name;
							int i,j;
							int unused_arg;	
							int reg;
							runtime_stack * start_point = stack_instance;

							TreeNode * arglist;
								while(stack_instance != NULL){
									if(!strcmp(stack_instance->function_name, label)) 
											break;
									
									stack_instance = stack_instance->next;
								}		
							numOfarg = (stack_instance->param_mem_size)/4;					
							if(arglist > 0){
								for(i = 0, arglist = AST->child[0]; i<numOfarg ; i++, arglist = arglist->sibling){
									unused_arg = get_arg_reg();

									if(arglist->is_array == 1 && arglist->child[0] == NULL){
									/*case of passing array*/
									//		printf("*****************%s\n", arglist->attr.name);
											fprintf(codefile,"\tli %s, %d\n", arg_name[unused_arg], arglist->memloc);
											continue;
									}
									reg = genExpK(arglist, codefile,stack_instance, right);
									fprintf(codefile,"\tmove %s, %s\n", arg_name[unused_arg],reg_name[reg]);
									used_reg[reg] = 0;					
								}
							}
									
								fprintf(codefile,"\tjal prog_%s\n", label);
								for(i = 0; i<numOfarg; i++){
									used_arg[i] = 0;		
								}
						
						stack_instance = start_point;
						}//end of else 
	 			 }		
				break;
			case DeclarK:
//				printf("D11\n");
				genDeclarK(AST, codefile, stack_instance);
					
				break;
		}
	
		if((AST->thiskind == ExpK && AST->attr.op == ASSIGN) || 
						(AST->thiskind == StmtK && AST->kind.stmt == SelectK) ||
							(AST->thiskind == StmtK && AST->kind.stmt == IterK) ){
		
		}
		else{
//			printf("D12\n");
			int i;
			// the other case ==> right child first
			for(i = 0; i < MAXCHILD; i++){
				
				codeGen(AST->child[i], codefile, stack_instance);	
			}
		}
	
	
	codeGen(AST->sibling, codefile, stack_instance);
	
	if(AST->thiskind == StmtK && AST->kind.exp == CompndK){
			function_flag -=1;
		if(function_flag == 1){
		
			DeallocStack(codefile, stack_instance);
			function_flag = 0;
		}	
	}
	}
	
} 

void genStmt(TreeNode* t, FILE * file, runtime_stack * stack)
{
	int label_one;
	int label_two;
	int label_three;
	int result_reg;
	
	switch(t->kind.stmt){
	
		case SelectK:
//				printf("D13\n");
				if(t->child[2] == NULL){
//				printf("D14\n");
						fprintf(file, "\n#if\n");
						label_one = gen_label(1);
						//label_two = gen_label(1);
						//result_reg stores the reusult value of the conditional expression
						result_reg = genExpK(t->child[0], file, stack, right);
						fprintf(file,"\tbeqz %s, L%d\n",reg_name[result_reg], label_one);
						codeGen(t->child[1],file, stack);
						fprintf(file,"L%d:\n",label_one);	
						//gen_label(0);
						used_reg[result_reg] = 0;
				}else{
//						printf("D15\n");
						fprintf(file, "\n#if-else\n");
						label_one = gen_label(1);
						label_two = gen_label(1);
						//result_reg stores the reusult value of the conditional expression
						result_reg = genExpK(t->child[0], file, stack, right);
						fprintf(file,"\tbeqz %s, L%d\n",reg_name[result_reg], label_two);
						codeGen(t->child[1], file, stack);
						fprintf(file,"\tj L%d\n",label_one);
						fprintf(file,"L%d:\n",label_two);
						codeGen(t->child[2], file, stack);	
						fprintf(file,"L%d:\n",label_one);
						//gen_label(0);
						//gen_label(0);
						used_reg[result_reg] = 0;
				}
				break;
		case IterK:
//						printf("D16\n");
						fprintf(file, "\n#while\n");
						label_one = gen_label(1);
						label_two = gen_label(1);
						fprintf(file, "\tj L%d\n",label_one);
						fprintf(file, "L%d:\n", label_two);
						codeGen(t->child[1], file, stack);
						fprintf(file, "L%d:\n",label_one);
						result_reg = genExpK(t->child[0], file, stack, right);
						fprintf(file, "\tbnez %s, L%d\n", reg_name[result_reg], label_two);
							

				break;
		
	}
}

int genExpK(TreeNode* t, FILE * file,runtime_stack * stack ,int lr_value)
{


 if(t != NULL){
	if(t->kind.exp == OpK){
//			printf("YEHA!\n");
			switch(t->attr.op){
//					printf("???\n");
					int l_reg;
					int r_reg;
					int unused_reg;
//					printf("funcking yaeh!\n");	
					case LTE :
//				   		printf("D17\n");	
						 l_reg = genExpK(t->child[0], file, stack ,left);
						 r_reg = genExpK(t->child[1], file, stack ,right);
						 unused_reg = get_reg();
						 fprintf(file,"\tsle %s, %s, %s\n", reg_name[unused_reg], reg_name[l_reg], reg_name[r_reg]);
						 used_reg[r_reg] = 0;
						 used_reg[l_reg] = 0;
						 return unused_reg;
						break;
					case LT :
//						 printf("D18\n");
						 l_reg = genExpK(t->child[0], file, stack, left);
						 r_reg = genExpK(t->child[1], file, stack, right);
						 unused_reg = get_reg();
						 fprintf(file,"\tslt %s, %s, %s\n", reg_name[unused_reg], reg_name[l_reg], reg_name[r_reg]);
						 used_reg[r_reg] = 0;
						 used_reg[l_reg] = 0;
						 return unused_reg;
						break;
					case GT : 
//						 printf("D19\n");
						 l_reg = genExpK(t->child[0], file, stack, left);
						 r_reg = genExpK(t->child[1], file, stack, right);
						 unused_reg = get_reg();
						 fprintf(file,"\tsgt %s, %s, %s\n", reg_name[unused_reg], reg_name[l_reg], reg_name[r_reg]);
						 used_reg[r_reg] = 0;
						 used_reg[l_reg] = 0;
						 return unused_reg;
						break;
					case GTE :
//						printf("D20\n");
						 l_reg = genExpK(t->child[0], file, stack,left);
						 r_reg = genExpK(t->child[1], file, stack,right);
						 unused_reg = get_reg();
						 fprintf(file,"\tsge %s, %s, %s\n", reg_name[unused_reg], reg_name[l_reg], reg_name[r_reg]);
						 used_reg[r_reg] = 0;
						 used_reg[l_reg] = 0;
						 return unused_reg;
						 break;
					case EQ:
//							printf("D21\n"); 
						 l_reg = genExpK(t->child[0], file, stack,left);
						 r_reg = genExpK(t->child[1], file, stack,right);
						 unused_reg = get_reg();
						 fprintf(file,"\tseq %s, %s, %s\n", reg_name[unused_reg], reg_name[l_reg], reg_name[r_reg]);
						 used_reg[r_reg] = 0;
						 used_reg[l_reg] = 0;
						 return unused_reg;
						break;
					case NEQ :
//printf("D22\n");
					   	 l_reg = genExpK(t->child[0], file, stack,left);
						 r_reg = genExpK(t->child[1], file, stack,right);
						 unused_reg = get_reg();
						 fprintf(file,"\tsne %s, %s, %s\n", reg_name[unused_reg], reg_name[l_reg], reg_name[r_reg]);
						 used_reg[r_reg] = 0;
						 used_reg[l_reg] = 0;
						 return unused_reg;
						 break;
					case PLUS :
//					printf("D23\n");				 
						 l_reg = genExpK(t->child[0], file, stack,left);
						 r_reg = genExpK(t->child[1], file, stack,right);
						 unused_reg = get_reg(used_reg);
						 fprintf(file,"\tadd %s, %s, %s\n", reg_name[unused_reg] ,reg_name[l_reg], reg_name[r_reg]);
						 used_reg[l_reg] = 0;
						 used_reg[r_reg] = 0;
						 return unused_reg;
						 break;	
					case MINUS :
//printf("D24\n");
					   	 l_reg = genExpK(t->child[0], file, stack,left);
						 r_reg = genExpK(t->child[1], file, stack,right);
						 unused_reg = get_reg();
						 fprintf(file,"\tsub %s, %s, %s\n", reg_name[unused_reg] ,reg_name[l_reg], reg_name[r_reg]);
						 used_reg[l_reg] = 0;
						 used_reg[r_reg] = 0;
						 return unused_reg;
						 break;

					case TIMES :
//	printf("D25\n");				
						 l_reg = genExpK(t->child[0], file, stack,left);
						 r_reg = genExpK(t->child[1], file, stack,right);
						 unused_reg = get_reg();
						 fprintf(file,"\tmul %s, %s, %s\n",reg_name[unused_reg] ,reg_name[l_reg], reg_name[r_reg]);
						 used_reg[l_reg] = 0;
						 used_reg[r_reg] = 0;
						 return unused_reg;
						 break;

					case DIVIDE:
//			printf("D26\n");		
						 l_reg = genExpK(t->child[0], file, stack,left);
						 r_reg = genExpK(t->child[1], file, stack,right);
						 unused_reg = get_reg();
						 fprintf(file,"\tdiv %s, %s, %s\n", reg_name[unused_reg] ,reg_name[l_reg], reg_name[r_reg]);
						 used_reg[l_reg] = 0;
						 used_reg[r_reg] = 0;
						 return unused_reg;
						break;
			}//end of switch-case
	}//end of if - case of Opk
	else if(t->kind.exp == IdK){
		int unused_reg;
		int index_val_reg;
		int array_val_reg;
		int array_base_reg;
		int offset_reg;
		int array_find_label;
		int array_not_find_label;
		int cur_fp_addr;
		///printf("%s\n",t->attr.name);
		/*have to consider if the value is array or not*/
		/*****************case of local variable****************/
		if(!t->global_var && lr_value == left){
			unused_reg = get_reg();
			//printf("here!\n");	
			if(t->is_array){//case of array variable
//			printf("D27\n");
				if(t->memloc > 0){
				//and the array is passed as parameter 	
				//step 1) find the base addresss of the array -> using fp and its offset stored in the parameter stack
//					printf("D28\n");
					used_reg[unused_reg] = 0;
					array_base_reg = get_reg();
					offset_reg = get_reg();
					array_find_label = gen_label(1);
					array_not_find_label = gen_label(1);
					fprintf(file, "\tlw %s, 0($fp)\n", reg_name[array_base_reg]);
					fprintf(file, "\taddi %s, $fp, %d\n", reg_name[offset_reg], t->memloc);
					
					//if not found, then branch to here!
					fprintf(file, "L%d:\n", array_not_find_label);
					
					fprintf(file, "\tlw %s, 0(%s)\n", reg_name[offset_reg], reg_name[offset_reg]);
					//fprintf(file, "\n\tmove $a0, %s\n\tli $v0, 1\n\tsyscall\n",reg_name[offset_reg]);	

					fprintf(file, "\tbltz %s, L%d\n",reg_name[offset_reg], array_find_label);
					fprintf(file, "\taddu %s, %s, %s\n", reg_name[offset_reg], reg_name[array_base_reg], reg_name[offset_reg]);
					fprintf(file, "\tlw %s, 0(%s)\n", reg_name[array_base_reg], reg_name[array_base_reg]);
					fprintf(file, "\tb L%d\n", array_not_find_label);
					//if found, then branch to here!
					fprintf(file, "L%d:\n", array_find_label);
					//if not works, then change add -> abs and subu
					fprintf(file, "\tneg %s, %s\n", reg_name[offset_reg], reg_name[offset_reg]);
					fprintf(file, "\tsubu %s, %s, %s\n",reg_name[array_base_reg], reg_name[array_base_reg], reg_name[offset_reg]);
					//deallocate the regs 
					used_reg[offset_reg] = 0;
					index_val_reg = genExpK(t->child[0],file,stack ,left);
				  fprintf(file, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
				  fprintf(file, "\taddu %s, %s, %s\n",reg_name[array_base_reg],reg_name[array_base_reg], reg_name[index_val_reg]);
				  used_reg[index_val_reg] = 0;
				  array_val_reg = get_reg();
				  fprintf(file, "\tlw %s, 0(%s)\n", reg_name[array_val_reg], reg_name[array_base_reg]);
				  used_reg[array_base_reg] = 0;
				  return array_val_reg;
				}
				else{
//printf("D29\n");
					//step 1 : store the base address temporarily to randomly selected register
					fprintf(file,"\tsubu %s, $fp, %d\n",reg_name[unused_reg], (t->memloc)*-1);
					//step 2 : get the register that stores a value of expression 	
			   		index_val_reg = genExpK(t->child[0],file,stack ,left);
					//step 3 : multiply 4 the value stored in the register
					fprintf(file, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
					//step 4 : move the register storing the base address of the array
					fprintf(file, "\taddu %s, %s, %s\n",reg_name[unused_reg],reg_name[unused_reg], reg_name[index_val_reg]);
					used_reg[index_val_reg] = 0;
					array_val_reg = get_reg();
					fprintf(file, "\tlw %s, 0(%s)\n", reg_name[array_val_reg], reg_name[unused_reg]);
					used_reg[unused_reg] = 0;
					return array_val_reg;
				}	
			}
			else{
//	printf("D30\n");			
				fprintf(file,"\tlw %s, %d($fp)\n",reg_name[unused_reg], t->memloc);
				return unused_reg;
			}
		}
		else if(!t->global_var && lr_value == right){
//		printf("D31\n");	
			unused_reg = get_reg();
			if(t->is_array){//case of array variable 
				if(t->memloc > 0){
//					printf("D32\n");			
				//and the array is passed as parameter 	
				//step 1) find the base addresss of the array -> using fp and its offset stored in the parameter stack
					used_reg[unused_reg] = 0;
					array_base_reg = get_reg();
					offset_reg = get_reg();
					array_find_label = gen_label(1);
					array_not_find_label = gen_label(1);
					fprintf(file, "\tlw %s, 0($fp)\n", reg_name[array_base_reg]);
					fprintf(file, "\taddi %s, $fp, %d\n", reg_name[offset_reg], t->memloc);
					
					//if not found, then branch to here!
					fprintf(file, "L%d:\n", array_not_find_label);
					
					fprintf(file, "\tlw %s, 0(%s)\n", reg_name[offset_reg], reg_name[offset_reg]);
					//fprintf(file, "\n\tmove $a0, %s\n\tli $v0, 1\n\tsyscall\n",reg_name[offset_reg]);	

					fprintf(file, "\tbltz %s, L%d\n",reg_name[offset_reg], array_find_label);
					fprintf(file, "\taddu %s, %s, %s\n", reg_name[offset_reg], reg_name[array_base_reg], reg_name[offset_reg]);
					fprintf(file, "\tlw %s, 0(%s)\n", reg_name[array_base_reg], reg_name[array_base_reg]);
					fprintf(file, "\tb L%d\n", array_not_find_label);
					//if found, then branch to here!
					fprintf(file, "L%d:\n", array_find_label);
					//if not works, then change add -> abs and subu
					fprintf(file, "\tneg %s, %s\n", reg_name[offset_reg], reg_name[offset_reg]);
					fprintf(file, "\tsubu %s, %s, %s\n",reg_name[array_base_reg], reg_name[array_base_reg], reg_name[offset_reg]);
					//deallocate the regs 
					used_reg[offset_reg] = 0;
					index_val_reg = genExpK(t->child[0],file,stack ,left);
				  fprintf(file, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
				  fprintf(file, "\taddu %s, %s, %s\n",reg_name[array_base_reg],reg_name[array_base_reg], reg_name[index_val_reg]);
				  used_reg[index_val_reg] = 0;
				  array_val_reg = get_reg();
				  fprintf(file, "\tlw %s, 0(%s)\n", reg_name[array_val_reg], reg_name[array_base_reg]);
				  used_reg[array_base_reg] = 0;
				  return array_val_reg;
				}
				else{
//printf("D33\n");
					//step 1 : store the base address temporarily to randomly selected register
					fprintf(file,"\tsubu %s, $fp, %d\n",reg_name[unused_reg], (t->memloc)*-1);
					//step 2 : get the register that stores a value of expression 	
			   		index_val_reg = genExpK(t->child[0],file, stack,left);
					//step 3 : multiply 4 the value stored in the register
					fprintf(file, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
					//step 4 : move the register storing the base address of the array
					fprintf(file, "\taddu %s, %s, %s\n",reg_name[unused_reg],reg_name[unused_reg], reg_name[index_val_reg]);
					used_reg[index_val_reg] = 0;
					array_val_reg = get_reg();
					fprintf(file, "\tlw %s, 0(%s)\n", reg_name[array_val_reg], reg_name[unused_reg]);
					return array_val_reg;	
				}
			}
			else{
//printf("D34\n");
				fprintf(file,"\tlw %s, %d($fp)\n",reg_name[unused_reg], t->memloc);
				return unused_reg;
			}
		}/************case of global variable***************/
		else if(t->global_var && lr_value == left){
//	printf("D35\n");		
			unused_reg = get_reg();
			if(t->is_array){//case of array variable 
				
//		printf("D36\n");			
					//step 1 : store the base address temporarily to randomly selected register
					fprintf(file,"\taddu %s, $gp, %d\n",reg_name[unused_reg], ((t->memloc)));
					//setp 2 : get the register that stores a value of expression 	
			   		index_val_reg = genExpK(t->child[0],file, stack,left);
					//step 3 : multiply 4 the value stored in the register
					fprintf(file, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
					//step 4 : move the register storing the base address of the array
					fprintf(file, "\tsubu %s, %s, %s\n",reg_name[unused_reg],reg_name[unused_reg], reg_name[index_val_reg]);
					used_reg[index_val_reg] = 0;
					array_val_reg = get_reg();
					fprintf(file,"\tlw %s, 0(%s)\n", reg_name[array_val_reg],reg_name[unused_reg]);
					used_reg[unused_reg] = 0;
					return array_val_reg;
					
			}
			else{
//printf("D37\n");
				int result_reg;
				result_reg = get_reg();
				fprintf(file,"\taddu %s, $gp, %d\n", reg_name[unused_reg], t->memloc);
				fprintf(file,"\tlw %s, 0(%s)\n", reg_name[result_reg], reg_name[unused_reg]);
				used_reg[unused_reg] = 0;
				//fprintf(file,"\tlw %s, %d\n", reg_name[unused_reg],t->memloc);
				return result_reg;
			}
		}
		else if(t->global_var && lr_value == right){
//	printf("D38\n");		
			unused_reg = get_reg();
			if(t->is_array){//case of array variable 
//		printf("D39\n");		
				//step 1 : store the base address temporarily to randomly selected register
				fprintf(file,"\taddu %s, $gp, %d\n",reg_name[unused_reg], ((t->memloc)));
				//setp 2 : get the register that stores a value of expression
			   	index_val_reg = genExpK(t->child[0],file, stack,left);
				//step 3 : multiply 4 the value stored in the register
				fprintf(file, "\tmul %s, %s, 4\n", reg_name[index_val_reg], reg_name[index_val_reg]);
				//step 4 : move the register storing the base address of the array
				fprintf(file, "\tsubu %s, %s, %s\n",reg_name[unused_reg],reg_name[unused_reg], reg_name[index_val_reg]);
				used_reg[index_val_reg] = 0;
				array_val_reg = get_reg();
				fprintf(file,"\tlw %s, 0(%s)\n", reg_name[array_val_reg],reg_name[unused_reg]);
				used_reg[unused_reg] = 0;
				return array_val_reg;	
			}
			else{
//				printf("D40\n");
				int result_reg;
				result_reg = get_reg();
				fprintf(file,"\taddu %s, $gp, %d\n", reg_name[unused_reg], t->memloc);
				fprintf(file,"\tlw %s, 0(%s)\n", reg_name[result_reg], reg_name[unused_reg]);
				used_reg[unused_reg] = 0;
				//fprintf(file,"\tlw %s, %d\n", reg_name[unused_reg],t->memloc);
				return result_reg;
			
			}
		}

	}//end of else if - case of IdK
	else if(t->kind.exp == ConstK){
	//	printf("D41\n");
		int unused_reg;
		if(lr_value == left){
			unused_reg = get_reg();		
			fprintf(file, "\tli %s, %d\n",reg_name[unused_reg], t->attr.val);
			return unused_reg;
		}
		else if(lr_value == right){
			unused_reg = get_reg();
			fprintf(file, "\tli %s, %d\n",reg_name[unused_reg], t->attr.val);	
			return unused_reg;
		}
	}//end of else if - case of ConstK
	else if(t->kind.exp == CallK){
		//printf("D42\n");
		int reg;	
		//case of function call of which return type is INT
			if(!strcmp(t->attr.name, "input")){
				 fprintf(file, "\n# input\n");
				 fprintf(file, "\tli $v0, 4\n");
				 fprintf(file, "\tla $a0, input_str\n");
				 fprintf(file, "\tsyscall\n");
				 // read_int
				 fprintf(file, "\tli $v0, 5\n");
				 fprintf(file, "\tsyscall\n");
				 //return value is stored in $v0
				 return 12;
				
			}else{
							int numOfarg;
							/*step 1 : find stack_instance*/
							char * label = t->attr.name;
							int i,j;
							int unused_arg;	
							int reg;
							TreeNode * arglist;
							runtime_stack *start_point = stack;
								while(stack != NULL){
									if(!strcmp(stack->function_name, label)) 
										break;
									stack = stack->next;
								}		
										
							numOfarg = (stack->param_mem_size)/4;					
							
								for(i = 0, arglist = t->child[0]; i<numOfarg ; i++, arglist = arglist->sibling){
									unused_arg = get_arg_reg();

									if(arglist->is_array == 1 && arglist->child[0] == NULL){
								//		fprintf(file,"\tli %s,892892\n", arg_name[unused_arg]);
								//			printf("************%s\n", arglist->attr.name);	
											fprintf(file,"\tli %s, %d\n", arg_name[unused_arg], arglist->memloc);

										//printf("***********%d\n",arglist->memloc);
										continue;

									}
									reg = genExpK(arglist, file, stack,right);
									fprintf(file,"\tmove %s, %s\n", arg_name[unused_arg],reg_name[reg]);
									used_reg[reg] = 0;						
								}	
								fprintf(file,"\tjal prog_%s\n", label);
								for(i = 0; i<numOfarg; i++){
									used_arg[i] = 0;		
								}
								//if there is any return value then it must be stored in $v0!
								stack = start_point;
								return 12;
			}

	}//end of else if - case of CallK	
 }//end of if - check whether t is null or not
}

void genDeclarK(TreeNode* t, FILE * file, runtime_stack * stack)
{

//	printf("10\n");
	if(t->kind.declar == FunDeclarK){

		/*step 1 : find stack_instance*/
		char * label = t->attr.name;
		int stack_size;
		int local_size;
		int param_size;
		int i;
		int numOfparam;
		int mem_offset;
		/*TO manage the stack allocation and deallocation of a function*/
		cur_function = t->attr.name;
		function_flag +=1;
		runtime_stack *start_point = stack;
//		printf("11\n");
		while(stack != NULL){
//			printf("name : %s\n", t->attr.name);				
//			printf("name : %s\n", stack->function_name);				
				if(!strcmp(stack->function_name, label)) 
						break;
		
		stack = stack->next;
		}
		//printf("12\n");
		/*step 2 : init var*/
		stack_size = stack->stack_size;
		local_size = stack->local_var_mem_size;
		param_size = stack->param_mem_size;
		//printf("13\n");
		fprintf(file,"#new procedure\n");
		if(!strcmp(label,"main")){
			fprintf(file,"%s:\n", label);
		}else{
			fprintf(file,"prog_%s:\n", label);
	
		}
		fprintf(file,"\tsubu $sp, $sp, %d\n", stack_size);
		fprintf(file,"\tsw $ra, %d($sp)\n", local_size);
		fprintf(file,"\tsw $fp, %d($sp)\n", (local_size+4));
		fprintf(file,"\taddu $fp, $sp, %d\n", local_size+4);
		
		numOfparam = param_size/4;
		mem_offset = param_size;
		//get the argument and store the value  to the param memory
		for(i = 0; i<numOfparam; i++){
		
			fprintf(file, "\tsw %s, %d($fp)\n", arg_name[i], (param_size-(4*i)));
		
		}
		stack = start_point;	
		//printf("14\n");
	}

}

void DeallocStack(FILE * file, runtime_stack * stack){

		/*step 1 : find stack_instance*/
		char * label = cur_function;
		int stack_size;
		int local_size;
		int param_size;

		while(stack != NULL){
			if(!strcmp(stack->function_name, label)) 
					break;
		
		stack = stack->next;
		}

		stack_size = stack->stack_size;
		local_size = stack->local_var_mem_size;
		param_size = stack->param_mem_size;

		fprintf(file,"#procedure out need to pop the stack\n");
		fprintf(file,"\tlw $ra, %d($sp)\n",local_size);
		fprintf(file,"\tlw $fp, %d($sp)\n", local_size+4);
		fprintf(file,"\taddu $sp, $sp, %d\n", stack_size);
		fprintf(file,"\tjr $ra\n");

		if(!strcmp(label, "main")){
			fprintf(file, "\tli $v0, 10\n");
			fprintf(file, "\tsyscall\n");
		}

}


void addIO(FILE *fp){

	fprintf(fp, ".data\n");
	fprintf(fp, "newline: .asciiz \"\\n\"\n");
	fprintf(fp, "output_str: .asciiz \"Output : \"\n");
	fprintf(fp, "input_str: .asciiz \"Input : \"\n");
    fprintf(fp, "\n.text\n");
}


