#include "globals.h"

#define NO_PARSE FALSE
#define NO_ANALYZE TRUE
#define NO_CODE FALSE

#include "parse.h"
#include "util.h"
#include "scan.h"
#include "analyze.h"
#include "cgen.h"
/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing;
FILE *code;

/*allocate and set tracing flags*/
int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

int main(int argc, char * argv[]){
	
    TreeNode * syntaxTree;
	runtime_stack * stack_instance = NULL;
	runtime_stack * search_s;

    char pgm[20];
    
    if (argc != 2){
       fprintf(stderr, "usage : %s <filename>\n", argv[0]);
       exit(1);	
    }

    listing = stdout;
   
    strcpy(pgm, argv[1]);
    if(strchr (pgm, '.') == NULL)
	  strcat(pgm, ".tny");
    source = fopen(pgm, "r");
    if (source == NULL)
    {
    	fprintf(stderr, "File %s not found\n", pgm);
	exit(1);
    }			
    listing = stdout;
	syntaxTree = parse();    
	if (TraceParse) {
//	   fprintf(listing, "\nSyntax tree: \n");
//	   printTree(syntaxTree);
	}


//	fprintf(listing, "\nBuilding Symbol Table...\n");
 	buildSymtab(syntaxTree);
	searchSymtab(syntaxTree, &stack_instance);
  	
	/*PROJECT 4*/
	while(stack_instance->prev !=NULL)
			stack_instance = stack_instance->prev;
/*	for(search_s = stack_instance; search_s != NULL; search_s = search_s->next){
			printf("LABEL : %s\n",search_s->function_name);
			printf("stack_size : %d\n", search_s->stack_size);
			printf("local_var size : %d\n", search_s->local_var_mem_size);
			printf("param_size : %d\n", search_s->param_mem_size);
	
	}*/
	
	char * codefile;
	int fnlen = strcspn(pgm,".");
	codefile = (char *) calloc(fnlen+4, sizeof(char));
	strncpy(codefile,pgm,fnlen);
	strcat(codefile,".tm");
//	printf("file name : %s\n", codefile);
	code = fopen(codefile,"w");
	if(code == NULL)
	{
		printf("Unable to open %s\n", codefile);
		exit(-1);
	}
	addIO(code);	
	codeGen(syntaxTree, code, stack_instance);
	fclose(code);
    

return 0;
}




