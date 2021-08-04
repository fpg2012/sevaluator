all: calc

calc.tab.c calc.tab.h:	calc.y
	bison -t -v -d calc.y

lex.yy.c: calc.l calc.tab.h
	flex calc.l

calc: lex.yy.c calc.tab.c calc.tab.h
	gcc -fPIC -shared -o libsevaluator.so calc.tab.c lex.yy.c -lgmp

test: lex.yy.c calc.tab.c calc.tab.h
	gcc -o test test.c calc.tab.c lex.yy.c -lgmp

clean:
	rm libsevaluator.so calc.tab.c lex.yy.c calc.tab.h calc.output test
