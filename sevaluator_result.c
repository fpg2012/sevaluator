#include "sevaluator_result.h"
#include <limits.h>
#include <string.h>

/*
    Z Q R (op1)
  Z Z Q R
  Q Q Q R
  R R R R
(op2)
*/
static ResultType _add_result_type(ResultType op1, ResultType op2) {
    if (op1 == R_INT && op2 == R_INT) {
        return R_INT;
    }
    else if (op1 == R_RAT || op2 == R_RAT && (op1 != R_FLT && op2 != R_FLT)) {
        return R_RAT;
    }
    return R_FLT;
}

/*
    Z Q R (op1)
  Z Q Q R
  Q Q Q R
  R R R R
(op2)
*/
static ResultType _div_result_type(ResultType op1, ResultType op2) {
    if (op1 != R_FLT && op2 != R_FLT) {
        return R_RAT;
    }
    return R_FLT;
}

/*
    Z Q R (op1)
  Z Z Q R
  Q R R R
  R R R R
(op2)
*/
static ResultType _pow_result_type(ResultType op1, ResultType op2) {
    if (op2 == R_INT) {
        return op1;
    }
    return R_FLT;
}

static int _cmp_type(ResultType rt1, ResultType rt2) {
    if (rt1 == rt2) {
        return 0;
    }
    if (rt1 < rt2) {
        return -1;
    }
    return 1;
}

// attention! this function does NOT call malloc for `result`
void sevaluator_result_init(FullResult *result, ResultType type) {
    result->result_type = type;
    if (type == R_INT) {
        mpz_init(result->result.v_int);
    }
    else if (type == R_RAT) {
        mpq_init(result->result.v_rat);
    }
    else {
        mpfr_init(result->result.v_flt);
    }
}

void sevaluator_result_init_str(FullResult *result, ResultType type, const char *str) {
    sevaluator_result_init(result, type);
    switch (type) {
        case R_INT:
            mpz_set_str(result->result.v_int, str, 10);
            break;
        case R_RAT:
            mpq_set_str(result->result.v_rat, str, 10);
            break;
        default:
            mpfr_set_str(result->result.v_flt, str, 10, MPFR_RNDN);
    }
}

// attention! this function does NOT free memory for `result`
void sevaluator_result_destroy(FullResult *result) {
    ResultType type = result->result_type;
    if (type == R_INT) {
        mpz_clear(result->result.v_int);
    }
    else if (type == R_RAT) {
        mpq_clear(result->result.v_rat);
    } else {
        mpfr_clear(result->result.v_flt);
    }
}

void sevaluator_result_copy(FullResult *result, FullResult *src) {
    sevaluator_result_init(result, src->result_type);
    if (src->result_type == R_INT) {
        mpz_set(result->result.v_int, src->result.v_int);
    } else if (src->result_type == R_RAT) {
        mpq_set(result->result.v_rat, src->result.v_rat);
    } else {
        mpfr_set(result->result.v_flt, src->result.v_flt, MPFR_RNDN);
    }
}

// do nothing if UNABLE to upgrade
void sevaluator_result_upgrade(FullResult *result, ResultType to_upgrade) {
    if (_cmp_type(result->result_type, to_upgrade) >= 0) {
        return;
    }
    // R_INT -> R_RAT
    if (to_upgrade == R_RAT) {
        result->result_type = R_RAT;

        mpq_t temp;
        mpq_init(temp);
        mpq_set_z(temp, result->result.v_int);

        mpz_clear(result->result.v_int);

        mpq_init(result->result.v_rat);
        mpq_set(result->result.v_rat, temp);

        mpq_clear(temp);
        return;
    }
    if (to_upgrade == R_FLT) {
        // R_RAT -> R_FLT
        if (result->result_type == R_RAT) {
            result->result_type = R_FLT;
            mpfr_t temp;
            mpfr_init(temp);
            mpfr_set_q(temp, result->result.v_rat, MPFR_RNDN);

            mpq_clear(result->result.v_rat);

            mpfr_init(result->result.v_flt);
            mpfr_set(result->result.v_flt, temp, MPFR_RNDN);

            mpfr_clear(temp);
            return;
        }
        // R_INT -> R_FLT
        else {
            result->result_type = R_FLT;
            mpfr_t temp;
            mpfr_init(temp);
            mpfr_set_z(temp, result->result.v_int, MPFR_RNDN);

            mpz_clear(result->result.v_int);

            mpfr_init(result->result.v_flt);
            mpfr_set(result->result.v_flt, temp, MPFR_RNDN);

            mpfr_clear(temp);
            return;
        }
    }
}

void sevaluator_result_add(FullResult *result, FullResult *op1, FullResult *op2) {
    ResultType result_type = _add_result_type(op1->result_type, op2->result_type);
    sevaluator_result_upgrade(op1, result_type);
    sevaluator_result_upgrade(op2, result_type);
    if (result_type == R_INT) {
        sevaluator_result_init(result, R_INT);
        mpz_add(result->result.v_int, op1->result.v_int, op2->result.v_int);
    }
    else if (result_type == R_RAT) {
        sevaluator_result_init(result, R_RAT);
        mpq_add(result->result.v_rat, op1->result.v_rat, op2->result.v_rat);
    }
    else {
        sevaluator_result_init(result, R_FLT);
        mpfr_add(result->result.v_flt, op1->result.v_flt, op2->result.v_flt, MPFR_RNDN);
    }
}

void sevaluator_result_sub(FullResult *result, FullResult *op1, FullResult *op2) {
    ResultType result_type = _add_result_type(op1->result_type, op2->result_type);
    sevaluator_result_upgrade(op1, result_type);
    sevaluator_result_upgrade(op2, result_type);
    if (result_type == R_INT) {
        sevaluator_result_init(result, R_INT);
        mpz_sub(result->result.v_int, op1->result.v_int, op2->result.v_int);
    }
    else if (result_type == R_RAT) {
        sevaluator_result_init(result, R_RAT);
        mpq_sub(result->result.v_rat, op1->result.v_rat, op2->result.v_rat);
    }
    else {
        sevaluator_result_init(result, R_FLT);
        mpfr_sub(result->result.v_flt, op1->result.v_flt, op2->result.v_flt, MPFR_RNDN);
    }
}

void sevaluator_result_mul(FullResult *result, FullResult *op1, FullResult *op2) {
    ResultType result_type = _add_result_type(op1->result_type, op2->result_type);
    sevaluator_result_upgrade(op1, result_type);
    sevaluator_result_upgrade(op2, result_type);
    if (result_type == R_INT) {
        sevaluator_result_init(result, R_INT);
        mpz_mul(result->result.v_int, op1->result.v_int, op2->result.v_int);
    }
    else if (result_type == R_RAT) {
        sevaluator_result_init(result, R_RAT);
        mpq_mul(result->result.v_rat, op1->result.v_rat, op2->result.v_rat);
    }
    else {
        sevaluator_result_init(result, R_FLT);
        mpfr_mul(result->result.v_flt, op1->result.v_flt, op2->result.v_flt, MPFR_RNDN);
    }
}

// this function does NOT check whether the `op2` is 0
void sevaluator_result_div(FullResult *result, FullResult *op1, FullResult *op2) {
    ResultType result_type = _div_result_type(op1->result_type, op2->result_type);
    sevaluator_result_upgrade(op1, result_type);
    sevaluator_result_upgrade(op2, result_type);
    if (result_type == R_RAT) {
        sevaluator_result_init(result, R_RAT);
        mpq_div(result->result.v_rat, op1->result.v_rat, op2->result.v_rat);
    }
    else {
        sevaluator_result_init(result, R_FLT);
        mpfr_div(result->result.v_flt, op1->result.v_flt, op2->result.v_flt, MPFR_RNDN);
    }
}

// this function does NOT check whether both `op1` and `op2` are integers
void sevaluator_result_mod(FullResult *result, FullResult *op1, FullResult *op2) {
    sevaluator_result_init(result, R_INT);
    mpz_mod(result->result.v_int, op1->result.v_int, op2->result.v_int);
}

void sevaluator_result_neg(FullResult *result, FullResult *op1) {
    sevaluator_result_init(result, op1->result_type);
    if (op1->result_type == R_INT) {
        mpz_neg(result->result.v_int, op1->result.v_int);
    } else if (op1->result_type == R_RAT) {
        mpq_neg(result->result.v_rat, op1->result.v_rat);
    } else {
        mpfr_neg(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
    }
}

// this function does NOT check whether `op1` is negative
void sevaluator_result_sqrt(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_sqrt(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_root_n(FullResult *result, FullResult *op1, unsigned long op2) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_rootn_ui(result->result.v_flt, op1->result.v_flt, op2, MPFR_RNDN);
}

void sevaluator_result_pow(FullResult *result, FullResult *op1, FullResult *op2) {
    ResultType result_type = _pow_result_type(op1->result_type, op2->result_type);
    int negexp_flag = 0;
    if (result_type == R_INT || result_type == R_RAT) {
        if (mpz_cmp_si(op2->result.v_int, 0) < 0)
        {
            result_type = R_RAT;
            negexp_flag = 1;
            mpz_neg(op2->result.v_int, op2->result.v_int);
        }
        if (mpz_cmp_ui(op2->result.v_int, ULONG_MAX) >= 0) 
        {
            result_type = R_FLT;
        }
    }
    if (result_type == R_INT) {
        unsigned long exp = mpz_get_ui(op2->result.v_int);
        sevaluator_result_init(result, R_INT);
        mpz_pow_ui(result->result.v_int, op1->result.v_int, exp);
    }
    else if (result_type == R_RAT) {
        sevaluator_result_upgrade(op1, R_RAT);
        unsigned long exp = mpz_get_ui(op2->result.v_int);
        mpz_pow_ui(mpq_numref(op1->result.v_rat), mpq_numref(op1->result.v_rat), exp);
        mpz_pow_ui(mpq_denref(op1->result.v_rat), mpq_denref(op1->result.v_rat), exp);
        sevaluator_result_init(result, R_RAT);
        if (negexp_flag) {
            mpq_set_num(result->result.v_rat, mpq_denref(op1->result.v_rat));
            mpq_set_den(result->result.v_rat, mpq_numref(op1->result.v_rat));
        } else {
            mpq_set(result->result.v_rat, op1->result.v_rat);
        }
    } else {
        sevaluator_result_upgrade(op1, R_FLT);
        if (op2->result_type == R_RAT) {
            unsigned long root = mpz_get_ui(mpq_denref(op2->result.v_rat));
            sevaluator_result_root_n(result, op1, root);
            mpfr_pow_z(result->result.v_flt, result->result.v_flt, mpq_numref(op2->result.v_rat), MPFR_RNDN);
        } else {
            sevaluator_result_init(result, R_FLT);
            mpfr_pow(result->result.v_flt, op1->result.v_flt, op2->result.v_flt, MPFR_RNDN);
        }
    }
}

void sevaluator_result_exp(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_exp(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_ln(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_log(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_log2(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_log2(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_log10(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_log10(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_sin(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_sin(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_cos(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_cos(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_tan(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_tan(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_cot(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_cot(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_sec(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_sec(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_csc(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_csc(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_atan(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_atan(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_asin(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_asin(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

void sevaluator_result_acos(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    sevaluator_result_init(result, R_FLT);
    mpfr_acos(result->result.v_flt, op1->result.v_flt, MPFR_RNDN);
}

int sevaluator_result_check_zero(FullResult *result) {
    if (result->result_type == R_INT) {
        return mpz_cmp_ui(result->result.v_int, 0);
    } else if (result->result_type == R_RAT) {
        return mpq_cmp_ui(result->result.v_rat, 0, 1);
    } else if (result->result_type == R_FLT) {
        return mpfr_cmp_ui(result->result.v_flt, 0);
    }
    return 0;
}

int sevaluator_result_cmp_si(FullResult *result, long si) {
    if (result->result_type == R_INT) {
        return mpz_cmp_si(result->result.v_int, si);
    } else if (result->result_type == R_RAT) {
        return mpq_cmp_si(result->result.v_rat, si, 1);
    } else if (result->result_type == R_FLT) {
        return mpfr_cmp_si(result->result.v_flt, si);
    }
    return 0;
}

char *sevaluator_result_get_str(FullResult *result, size_t digits) {
    char *temp;
    if (result->result_type == R_INT) {
        temp = mpz_get_str(NULL, 10, result->result.v_int);
        return temp;
    } else if (result->result_type == R_RAT) {
        temp = mpq_get_str(NULL, 10, result->result.v_rat);
        
        int flag = 0;
        mpz_t a;
        mpz_init(a);
        mpq_get_den(a, result->result.v_rat);
        char *temp_a = mpz_get_str(NULL, 10, a);
        char *p = temp_a;
        if (*p == '1') {
            flag = 1;
        }
        ++p;
        while(*p && flag) {
            if (*p == '0') {
                ++p;
            } else {
                flag = 0;
            }
        }
        free(temp_a);
        if (flag || strlen(temp) > 20)
        {
            sevaluator_result_upgrade(result, R_FLT);
            free(temp);
            temp = (char *)malloc(1000000);
            mpfr_sprintf(temp, "%Rf", result->result.v_flt);
            char *temp2 = (char *)malloc(strlen(temp) + 1);
            strcpy(temp2, temp);
            free(temp);
            temp = temp2;
        }
        mpz_clear(a);
        return temp;
    } else {
        char *temp = (char*) malloc(1000000);
        mpfr_sprintf(temp, "%Rf", result->result.v_flt);
        char *temp2 = (char*) malloc(strlen(temp) + 1);
        strcpy(temp2, temp);
        free(temp);
        return temp2;
    }
    return NULL;
}