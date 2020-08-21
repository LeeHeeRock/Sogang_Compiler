#include "globals.h"
#include "util.h"


void printToken(TokenType token, const char* tokenString){

	int len, i;
	char * reserved[48] = { "empty", "empty",
				"if", "else", "int", "return", "void", "while",
				"id","num",
				"plus","minus","times", "divide", 
				"lt", "lte", "gt", "gte", "eq", "neq", 
				"assign", 
				"semi","com", "lparen", "rparen", "lsqbr","rsqbr", "lcrbr", "rcrbr",
				"+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=",
				"=",
				";", ",", "(", ")", "[", "]", "{", "}"
			      };
	char * temp;
	switch (token)
	{	
		
		case IF: case ELSE:
			
		case INT: case RETURN:
			
		case VOID: case WHILE:
			len = strlen(reserved[token]);
			temp = (char*)malloc(sizeof(char)*(len+1));
			temp[len+1] = '\0';
			for(i=0; i<len; i++){temp[i] = toupper(reserved[token][i]);}
			
			fprintf(listing, "\t\t%s\t\t%s\n",temp, tokenString);
			free(temp);
			break;
		
		case PLUS:  case MINUS:
		
		case TIMES: case DIVIDE:
		
		case LT: case LTE:
		
		case GT: case GTE:
		
		case EQ: case NEQ:
		
		case ASSIGN: case SEMI:
		
		case COM: case LPAREN:
		
		case RPAREN: case LSQBR:
		
		case RSQBR: case LCRBR:
		
		case RCRBR:

			len = strlen(reserved[token]);
			temp = (char*)malloc(sizeof(char)*(len+1));
			for(i=0; i<len; i++){temp[i] = toupper(reserved[token][i]);}
			temp[len] = '\0';	
			fprintf(listing, "\t\t%s\t\t%s\n",temp, reserved[token + symb_dis]);
			free(temp);
			break;
		
		case ID:
			fprintf(listing, "\t\tID\t\t%s\n",tokenString); break;	
		case NUM:
			fprintf(listing, "\t\tNUM\t\t%s\n",tokenString); break;		
		case ENDFILE:
			fprintf(listing,"\t\tENDFILE\t\tEOF\n"); break;
		case CMTERR:
			fprintf(listing,"\t\tERROR\t\tComment Error\n");break;
		default:
			fprintf(listing, "\t\t ERROR\t\t%s\n",tokenString);
	}
}
