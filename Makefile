all: calc

calc.tab.c calc.tab.h:	calc.y
	bison -t -v -d calc.y -Wcounterexamples

lex.yy.c: calc.l calc.tab.h
	flex calc.l

calc: lex.yy.c calc.tab.c calc.tab.h sevaluator_history.c sevaluator_result.c
	gcc -fPIC -shared -o libsevaluator.so calc.tab.c lex.yy.c sevaluator_history.c -lgmp -lmpfr

test: lex.yy.c calc.tab.c calc.tab.h sevaluator_history.c sevaluator_result.c test.c
	gcc -g -o test test.c calc.tab.c lex.yy.c sevaluator_history.c sevaluator_result.c -lgmp -lmpfr

clean:
	rm libsevaluator.so calc.tab.c lex.yy.c calc.tab.h calc.output test
