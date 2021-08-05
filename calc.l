%option noyywrap

%{
#include <stdio.h>
#include <gmp.h>
#include <string.h>
#include <stdbool.h>
#define YY_DECL int yylex()
#include "sevaluator_result.h"
#include "calc.tab.h"

void from_decimal(mpq_t, const char *);
%}

%%

[ \t\n\r] ;
[0-9]+\.[0-9]+ { 
    from_decimal(yylval.v_rational, yytext); 
    return RATIONAL; 
    }
[0-9]+ {
    mpz_init(yylval.v_integer);
    mpz_set_str(yylval.v_integer, yytext, 10);
    return INTEGER;
    }
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MULTIPLY; }
"/" { return DIVIDE; }
"^" { return POWER; }
"%" { return MOD; }
"√" { return SQRT; }
"!" { return FACTORIAL; }
"(" { return LEFT; }
")" { return RIGHT; }
"[" { return F_LEFT; }
"]" { return F_RIGHT; }
"," { return COMMA; }
"hist" { return FUNC_HIST; }
"random" { return FUNC_RANDOM; }
"ans" { return ANS; }

%%

void from_decimal(mpq_t num, const char *buf) {
    size_t len = strlen(buf);
    len = len * 2 + 10;
    char *new_str = (char*) calloc(1, len);
    int count_zero = 0;
    const char *p = buf;
    char *q = new_str;
    bool after_dot = false;
    while(*p) {
        if (*p == '.') {
            after_dot = true;
            p++;
            continue;
        }
        *q = *p;
        if (after_dot) {
            count_zero++;
        }
        p++; q++;
    }
    *q++ = '/'; *q++ = '1';
    for (int i = 0; i < count_zero; ++i) {
        *q = '0';
        q++;
    }
    mpq_init(num);
    mpq_set_str(num, new_str, 10);
    free(new_str);
    mpq_canonicalize(num);
}