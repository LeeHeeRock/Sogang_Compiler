#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER
#include "tiny.tab.h"
#endif

//#ifndef ENDFILE
//#define ENDFILE 0
//#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/*MAXRESERVED = the number of reserved words*/

#define MAXRESERVEDWORD 6
#define MAXCHILD 3

/*NO USE IN PROJECT 2*/
/*typedef enum
{ENDFILE,ERROR,IF, ELSE, INT, RETURN, VOID, WHILE, ID, NUM,

PLUS, MINUS, TIMES, DIVIDE, LT, LTE, GT, GTE, EQ, NEQ, ASSIGN, 

SEMI, COM, LPAREN, RPAREN, LSQBR, RSQBR, LCRBR, RCRBR, CMTERR, CMTERRTWO
}TokenType;
*/
typedef int TokenType;

typedef enum {StmtK, ExpK, DeclarK} NodeKind;
typedef enum {CompndK, SelectK, IterK, ReturnK} StmtKind;
typedef enum {VarDeclarK,  FunDeclarK} DeclarKind;
typedef enum {OpK, ConstK, IdK, TypeK, ParamK, CallK} ExpKind;
typedef enum {Int, Void, Array} Types;

struct Type{
	Types in_type;
	int in_size;
};

typedef struct TreeNode
{
	struct TreeNode * child[MAXCHILD];
	struct TreeNode * sibling;

	NodeKind thiskind;
	int lineno;

	union{StmtKind stmt; ExpKind exp; DeclarKind declar;}kind;
	union{struct Type type; int op; int val; char * name; }attr;

}TreeNode;

extern FILE* source;
extern FILE* listing;
extern FILE* code;

extern int lineno;
extern int TraceScan;
extern int TraceAnalyze;
extern int TraceCode;
extern int EchoSource;
extern int Error;
#endif

