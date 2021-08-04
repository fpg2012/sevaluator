all: calc

calc.tab.c calc.tab.h:	calc.y
	bison -t -v -d calc.y

lex.yy.c: calc.l calc.tab.h
	flex calc.l

calc: lex.yy.c calc.tab.c calc.tab.h
	gcc -fPIC -shared -o libsevaluator.so calc.tab.c lex.yy.c sevaluator_history.c -lgmp

test: lex.yy.c calc.tab.c calc.tab.h sevaluator_history.c test.c
	gcc -g -o test test.c calc.tab.c lex.yy.c sevaluator_history.c -lgmp

clean:
	rm libsevaluator.so calc.tab.c lex.yy.c calc.tab.h calc.output test
