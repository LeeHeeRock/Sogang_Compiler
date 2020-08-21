%{
#undef yywrap
#include "globals.h"
#include "util.h"
#include "scan.h"

char tokenString[MAXTOKENLEN+1];
int cmtStart;
int flag_cmterr = 0;
%}

%x CMT
%x NCMT
	
digit [0-9]
number {digit}+
letter [a-zA-Z]
identifier {letter}+

newline \n
tab     \t
whitespace [ \t\n]

quto "/*"


%%

"/*"   	{ /*cmtStart = lineno;*/
          BEGIN CMT;		
	 /*Enter Comment eating state*/
	}

<CMT>"*/" 	  {BEGIN 0;}
<CMT>\n   	  {lineno++;}
<CMT>.		  {/*skip*/}
<CMT>.\n          {lineno++;}

<CMT>"/*"  	  {BEGIN NCMT;}
<CMT><<EOF>> 	  {BEGIN 0; flag_cmterr = 1; return CMTERR;}

<NCMT><<EOF>>	  {BEGIN 0; flag_cmterr = 1; return CMTERR;}	     
<NCMT>\n	  {lineno++;}
<NCMT>.		  {/*skip*/}
<NCMT>.\n         {lineno++;}

<NCMT>"*/"	  {BEGIN 0;}	


"if"	{return IF;}
"else"	{return ELSE;}
"int"	{return INT;}
"return"	{return RETURN;}
"void"	{return VOID;}
"while"	{return WHILE;}
"+"	{return PLUS;}
"-"	{return MINUS;}
"*/"	{return ERROR;}
"*"	{return TIMES;}
"/"	{return DIVIDE;}
"<"	{return LT;}
"<="	{return LTE;}
">"	{return GT;}
">="	{return GTE;}
"=="	{return EQ;}
"!="	{return NEQ;}
"="	{return ASSIGN;}
";"	{return SEMI;}
","	{return COM;}
"("	{return LPAREN;}
")"	{return RPAREN;}
"["	{return LSQBR;}
"]"	{return RSQBR;}
"{"	{return LCRBR;}
"}"	{return RCRBR;}

{number} {return NUM;}
{identifier} {return ID;}
{newline} {lineno++;}
{whitespace} {}

. 	{return ERROR;}
%%

TokenType getToken(void)
{
	static int firstTime = TRUE;
	TokenType currentToken;
	if(firstTime)
	{
		firstTime = FALSE;
		lineno++;
		yyin = source;
		yyout = listing;
	}
	currentToken = yylex();
	strncpy(tokenString, yytext, MAXTOKENLEN);
	if(TraceScan){
		/*if(currentToken == CMTERR) 
		   fprintf(listing, "    %d     ",cmtStart);
		else*/
		fprintf(listing, "    %d     ",lineno);
		printToken(currentToken, tokenString);
	}
	return currentToken;
}
/*  <NCMT>."*//*"{newline}*.*{newline}*"*//*"+  {BEGIN 0; flag_cmterr = 1; return CMTERR;}
*/

