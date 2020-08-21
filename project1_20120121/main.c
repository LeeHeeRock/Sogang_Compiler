#include "globals.h"

#define NO_PARSE TRUE
#define NO_ANALYZE TRUE
#define NO_CODE TRUE

#include "util.h"
#include "scan.h"

/*NOT USE IN PROJ 1*/

/*#if NO_PRASE
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif
*/

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

    //TreeNode * syntaxTree;
    char pgm[20];
    
    if (argc != 2){
       fprintf(stderr, "usage : %s <filename>\n", argv[0]);
       exit(1);	
    }

    listing = stdout;
    fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);

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
    fprintf(listing, "\nTINY COMPILATION : %s\n", pgm);

#if NO_PARSE
	fprintf(listing,"line number 		token		lexeme\n");
	fprintf(listing,"---------------------------------------------\n");	
	while(getToken()!=ENDFILE);
/*NOT USE IN PROJ 1*/
/*#else
 	stntaxTree = parse();
        if (TraceParse) {
	   fprintf(listing, "\nSyntax tree: \n");
	   printTree(stntaxTree);
	}
#if !NO_ANALYZE
    if (!Error)
    {
	
	fprintf(listing, "\nBuilding Symbol Table...\n");
 	buildSymtab(stntaxTree);
	fprintf(listing, "\nChecking Types...\n");
 	typeCheck(stntaxTree);
	fprintf(listing,"\nType Checking Finished\n"):
    }
#if !NO_CODE
    if (!Error)
    {
	char * codefile;
	int fnlen = strcspn(pgm,".");
	codefile = (char *) calloc(fnlen+4, sizeof(char));
	strncpy(codefile,pgm,fnlen);
	strcat(codefile,".tm")
	code = fopen(codefile,"w");
	if(code == NULL)
	{
		printf("Unable to open %s\n", codefile);
		exit();
	}
	codeGen(syntaxTree, codefile);
	fclose(code);
    }
#endif    	
#endif*/
#endif
 return 0;
}




