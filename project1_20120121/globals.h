#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/*MAXRESERVED = the number of reserved words*/

#define MAXRESERVEDWORD 6
typedef enum
{
/*book-keeping tokens*/
ENDFILE,ERROR,
/*reserved words*/
IF, ELSE, INT, RETURN, VOID, WHILE,
/*multicharacter tokens*/
ID, NUM,
/*special symbols*/
/*+ - * / < <= > >= == != = ; , ( ) [] {}  */
PLUS, MINUS, TIMES, DIVIDE, 

LT, LTE, GT, GTE, EQ, NEQ, 

ASSIGN, 

SEMI, COM, LPAREN, RPAREN, LSQBR, RSQBR, LCRBR, RCRBR,

CMTERR, CMTERRTWO
}TokenType;
 
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

