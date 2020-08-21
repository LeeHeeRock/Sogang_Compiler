#ifndef _SCAN_H
#define _SCAN_H

#define MAXTOKENLEN 40


extern char tokenString[MAXTOKENLEN+1];

TokenType getToken();
static int yylex(void);
int yyerror(char *);

#endif
