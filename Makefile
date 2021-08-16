all: sevaluator

sevaluator_calc.tab.c sevaluator_calc.tab.h:	sevaluator_calc.y
	bison -t -v -d sevaluator_calc.y -Wcounterexamples

lex.yy.c: sevaluator_calc.l sevaluator_calc.tab.h
	flex sevaluator_calc.l

sevaluator: lex.yy.c sevaluator_calc.tab.c sevaluator_calc.tab.h sevaluator.h sevaluator_history.h sevaluator_result.h sevaluator_history.c sevaluator_result.c
	gcc -fPIC -shared -o libsevaluator.so sevaluator_calc.tab.c lex.yy.c sevaluator_history.c sevaluator_result.c -lgmp -lmpfr

test: lex.yy.c sevaluator_calc.tab.c sevaluator_calc.tab.h sevaluator.h sevaluator_history.h sevaluator_result.h sevaluator_history.c sevaluator_result.c test.c
	gcc -g -o test test.c sevaluator_calc.tab.c lex.yy.c sevaluator_history.c sevaluator_result.c -lgmp -lmpfr

clean:
	rm libsevaluator.so sevaluator_calc.tab.c lex.yy.c sevaluator_calc.tab.h sevaluator_calc.output test
