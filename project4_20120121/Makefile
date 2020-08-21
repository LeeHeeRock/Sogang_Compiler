project4_26: tiny.tab.o main.o util.o lex.yy.o analyze.o symtab.o cgen.o
	gcc -o project4_26 main.o util.o analyze.o lex.yy.o tiny.tab.o symtab.o cgen.o -lfl

main.o: main.c globals.h util.h scan.h analyze.h cgen.h
	gcc -c main.c

util.o: util.c util.h globals.h
	gcc -c util.c

analyze.o: analyze.c globals.h symtab.h analyze.h 
	gcc -c analyze.c

symtab.o: symtab.c symtab.h
	gcc -c symtab.c
cgen.o : cgen.c cgen.h
	gcc -c cgen.c

lex.yy.o: lex.yy.c util.h globals.h scan.h
	gcc -c lex.yy.c

lex.yy.c: tiny.l
	flex tiny.l

tiny.tab.o : tiny.tab.c tiny.tab.h 
	gcc -c tiny.tab.c

tiny.tab.c tiny.tab.h: tiny.y
	bison -v -d tiny.y

clean:
	rm project4_26 lex.yy.c tiny.tab.* tiny.output *.o
