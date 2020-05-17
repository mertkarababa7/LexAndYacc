LEX = lex
YACC = yacc -d

CC = cc -std=c99

calculator: y.tab.o lex.yy.o functions.o calculator.o
		$(CC) -o $@ y.tab.o lex.yy.o  functions.o calculator.o #-ll -lm

calculator.o: calculator.c

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: parser.y
	$(YACC) -v parser.y

lex.yy.c: lexer.l
	$(LEX) lexer.l

functions.o: functions.c

clean:
	-rm -f *.o lex.yy.c *.tab.*  calculator *.output

