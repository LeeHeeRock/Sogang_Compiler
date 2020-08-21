20120121: main.o util.o lex.yy.o 
	gcc -o 20120121 main.o util.o lex.yy.o -lfl

main.o: main.c globals.h util.h scan.h
	gcc -c main.c

util.o: util.c util.h globals.h
	gcc -c util.c

lex.yy.o: lex.yy.c util.h globals.h scan.h
	gcc -c lex.yy.c

lex.yy.c: tiny.l
	flex tiny.l

clean:
	rm *.o 20120121 lex.yy.c	
