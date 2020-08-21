%{
/*distinguishes YACC output form other cod files*/
#define YYPARSER
#include <string.h>
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *

static char * savedName;
static int savedLineNo;
static int savedNumber;
static TreeNode * savedTree;
int tmpLen;
%}

/*%token IF ELSE INT RETURN VOID WHILE*/
%token ID NUM
%token SEMI COM LPAREN RPAREN LSQBR RSQBR LCRBR RCRBR
%token IF ELSE INT RETURN VOID WHILE
%token ASSIGN
%token LT LTE GT GTE EQ NEQ 
%token TIMES DIVIDE 
%token PLUS MINUS  
%token ERROR CMTERR
%token ENDFILE

%left TIMES
%left DIVIDE

%left PLUS
%left MINUS

%left LT 
%left LTE 
%left GT 
%left GTE 
%left EQ 
%left NEQ

/*a = a+b*/
%right ASSIGN

%right RPAREN ELSE


%%

program 		: declar_list
				{ savedTree = $1;}
				;

declar_list 	: declar_list declar
				{ 
			  		YYSTYPE t = $1;
			  		if ( t != NULL )
			  		{
						while(t->sibling != NULL)
							t = t->sibling;
						t->sibling = $2;
						$$ = $1;
			 		 }
			  		 else $$ = $2; 
				}
				| declar { $$ = $1;}
				;

declar 			: var_declar { $$ = $1;}
				| fun_declar { $$ = $1;}
				;

var_declar  	: type_specifier var_name SEMI{
					
					$$ = newDeclarNode(VarDeclarK);
					$$->child[0] = $1;
					get_Name($$, $2);
			  	}
			 
				| type_specifier var_name LSQBR var_size RSQBR SEMI{
					$$ = newDeclarNode(VarDeclarK);
					$$->child[0] = $1;
					get_Name($$, $2);	
			    	get_Size($$, $4);		
		       	}
			 	;
fun_declar      : type_specifier var_name LPAREN params RPAREN compound_stmt{
			
					$$ = newDeclarNode(FunDeclarK);
					$$->child[0] = $1;
					$$->child[1] = $4;
					$$->child[2] = $6;				
					get_Name($$, $2);
				}
				;

params			: param_list{$$ = $1;}
/*TODO- check whether there can be any void type variable in paramlist*/
				| VOID		{
				
				   $$ = newExpNode(ParamK);
				   $$->attr.type.in_type = Void;
				   $$->attr.type.in_size = -1;

				}
				;

param_list			: param_list COM param{
						YYSTYPE t = $1;
			  			if ( t != NULL )
			  			{
							while(t->sibling != NULL)
								t = t->sibling;
							t->sibling = $3;
							$$ = $1;
			 		 	}
			  			 else $$ = $3;
					}
					| param {$$ = $1;}
					;

param				: type_specifier param_name	{
				
				  		$$ = newExpNode(ParamK);
				  		$$->child[0] = $1;
				  		get_Name($$, $2);								
					}
					| type_specifier param_name LSQBR RSQBR	{
				  
				  		$$ = newExpNode(ParamK);
				  		$$->child[0] = $1;
				  /*Indicates that it is array parameter*/
				  		$$->child[0]->attr.type.in_size = 1;
				  		get_Name($$, $2);
					} 
					;

type_specifier 		: INT   { 
			   				$$ = newExpNode(TypeK);
			 		    	$$->attr.type.in_type = Int; 
							$$->attr.type.in_size = 0;
					} 
			   		| VOID  {
							$$ = newExpNode(TypeK);
			 		    	$$->attr.type.in_type = Void; 
							$$->attr.type.in_size = 0;
					}
					;

var_name 	    	: ID 	{ 
			  				$$ = newDeclarNode(VarDeclarK);
			 		  		$$->attr.name = copyString(tokenString);
						  	$$->lineno = lineno; 
					}
					;

var_size	    	: NUM	{ 
			 				$$ = newDeclarNode(VarDeclarK);
					  		$$->attr.val = atoi(tokenString);
					}
					;
param_name 			: ID 	{
			 				$$ = newDeclarNode(VarDeclarK);
			 		  		$$->attr.name = copyString(tokenString);
						  	$$->lineno = lineno;
					}
					;
compound_stmt		: LCRBR local_declar statement_list RCRBR{
						$$ = newStmtNode(CompndK);
						$$->child[0] = $2;
						$$->child[1] = $3;
					}
					;

local_declar 		: local_declar var_declar{
			  			YYSTYPE t = $1;
			  			if ( t != NULL )
			  			{
							while(t->sibling != NULL)
								t = t->sibling;
							t->sibling = $2;
							$$ = $1;
			 			}
			  			else $$ = $2;
					}
					| empty{$$ = $1;}
					;
statement_list  	: statement_list statement{
						YYSTYPE t = $1;
			  			if ( t != NULL )
			  			{
							while(t->sibling != NULL)
								t = t->sibling;
							t->sibling = $2;
							$$ = $1;
			 			}
			  			else $$ = $2;
					}
					| empty{ 
							$$ = $1;
					}
					;
statement 			: expression_stmt{
						$$ = $1;
					}
					| compound_stmt{
						$$ = $1;
					}
					| selection_stmt{
						$$ = $1;
					}
					| iteration_stmt{
						$$ = $1;
					}
					| return_stmt{
						$$ = $1;
					}  
					;
expression_stmt 	: expression SEMI{
						$$ = $1;
					}
					| SEMI{
						$$ = NULL;
					}
					;
expression      	: ex_var ASSIGN expression{
/* make a expression statement node and allocate proper values */
						$$ = newExpNode(OpK);
						$$->attr.op = ASSIGN;
						$$->child[0] = $1;
						$$->child[1] = $3; 
					}
			   		| simple_expression{
						$$ = $1;					
					}
					;
ex_var				: ex_ID{
		  				$$ = newExpNode(IdK);
						get_Name($$,$1);
					}
					| ex_ID LSQBR expression RSQBR{
						$$ = newExpNode(IdK);
						get_Name($$, $1);
						$$->child[0] = $3;
					}
					;
ex_ID				: ID{
		 				$$ = newExpNode(IdK);
						$$->attr.name = copyString(tokenString);
					}
					;
simple_expression 	: additive_expression relop additive_expression{
				 		$$ = newExpNode(OpK);
						$$->attr.op = $2->attr.op;
						free($2);
						$$->child[0] = $1;
						$$->child[1] = $3;
  				  	}
				 	| additive_expression{
						$$ = $1;
				  	}
				  	;
additive_expression : additive_expression addop term{
						$$ = newExpNode(OpK);
						$$->attr.op = $2->attr.op;
						free($2);
						$$->child[0] = $1;
						$$->child[1] = $3;				
					}
					| term{
						$$ = $1;
					}
					;
selection_stmt		: IF LPAREN expression RPAREN statement{
						$$ = newStmtNode(SelectK);
						$$->child[0] = $3;
						$$->child[1] = $5;					
					}
					| IF LPAREN expression RPAREN statement ELSE statement{
						$$ = newStmtNode(SelectK);
						$$->child[0] = $3;
						$$->child[1] = $5;
						$$->child[2] = $7;
					}
					;
return_stmt			: RETURN SEMI{
			  			$$ = newStmtNode(ReturnK);	
					}
			  		| RETURN expression SEMI{
						$$ = newStmtNode(ReturnK);
						$$->child[0] = $2;
					}
iteration_stmt      : WHILE LPAREN expression RPAREN statement{
						$$ = newStmtNode(IterK);
						$$->child[0] = $3;
						$$->child[1] = $5;	
					}
					;
term 				: term mulop factor{

						$$ = newExpNode(OpK);
						$$->attr.op = $2->attr.op;
						$$->child[0] = $1;
						$$->child[1] = $3;
	
					}
					| factor{
						$$ = $1;
					}
					;
factor 				: LPAREN expression RPAREN{
		   				$$ = $2;
					}
					| ex_var{
						$$ = $1;
					}
					| call{
						$$ = $1;

					}
					| NUM{
						$$ = newExpNode(ConstK);
					  	$$->attr.val = atoi(tokenString);
					}
					;
call				: ex_ID LPAREN args RPAREN{
						$$ = newExpNode(CallK);
						get_Name($$, $1);
						$$->child[0] = $3;						
					}
					;
args				: arg_list{
						$$ = $1;
					}
					| empty{
						$$ = $1;
					}
					;
arg_list			: arg_list COM expression{
		   				YYSTYPE t = $1;
			  			if ( t != NULL )
			  			{
							while(t->sibling != NULL)
								t = t->sibling;
							t->sibling = $3;
							$$ = $1;
			 			}
			  			else $$ = $3;
					}
					|expression{
						$$ = $1;

					}
mulop				: TIMES{
		 				$$ = newExpNode(OpK);
						$$->attr.val = TIMES;
						
					}
		 			| DIVIDE{
						$$ = newExpNode(OpK);
						$$->attr.val = DIVIDE;
					}
					;

addop				: PLUS{
		 				$$ = newExpNode(OpK);
						$$->attr.val = PLUS;

					}
					| MINUS{
						$$ = newExpNode(OpK);
						$$->attr.val = MINUS;

					}
					;

relop				: GT{
		 				$$ = newExpNode(OpK);
						$$->attr.val = GT;
					}
					| LT{
						$$ = newExpNode(OpK);
						$$->attr.val = LT;
					}
					| GTE{
						$$ = newExpNode(OpK);
						$$->attr.val = GTE;
					}
					| LTE{
						$$ = newExpNode(OpK);
						$$->attr.val = LTE;
					}
					| EQ{
						$$ = newExpNode(OpK);
						$$->attr.val = EQ;
					}
					| NEQ{
						$$ = newExpNode(OpK);
						$$->attr.val = NEQ;
					}
					; 	
empty 				: {	$$ = NULL;};

%%
int yyerror(char * message){
	
	fprintf(listing, "Syntax error at line %d: %s\n", lineno, message);
	fprintf(listing, "Current token : ");
	printToken(yychar, tokenString);
	Error = TRUE;
	return 0;
}

static int yylex(){
		
	TokenType token;
	token = getToken();
	return token;
}
             
TreeNode * parse(){
	
	yyparse();
	
	return savedTree;
}


