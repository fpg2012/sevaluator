#include "sevaluator_result.h"

/*
    Z Q R (op1)
  Z Z Q R
  Q Q Q R
  R R R R
(op2)
*/
static ResultType _add_result_type(ResultType op1, ResultType op2) {
    if (op1 == R_INT && op2 == R_RAT) {
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
    mpf_init(result->result.v_flt);
}

// attention! this function does NOT free memory for `result`
void sevaluator_result_destroy(FullResult *result) {
    ResultType type = result->result_type;
    if (type == R_INT) {
        mpz_clear(result->result.v_int);
    }
    else if (type == R_RAT) {
        mpq_clear(result->result.v_rat);
    }
    mpf_clear(result->result.v_flt);
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
            mpf_t temp;
            mpf_init(temp);
            mpf_set_q(temp, result->result.v_rat);

            mpq_clear(result->result.v_rat);

            mpf_init(result->result.v_flt);
            mpf_set(result->result.v_flt, temp);

            mpf_clear(temp);
            return;
        }
        // R_INT -> R_FLT
        else {
            result->result_type = R_FLT;
            mpf_t temp;
            mpf_init(temp);
            mpf_set_z(temp, result->result.v_int);

            mpz_clear(result->result.v_int);

            mpf_init(result->result.v_flt);
            mpf_set(result->result.v_flt, temp);

            mpf_clear(temp);
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
        mpf_add(result->result.v_flt, op1->result.v_flt, op2->result.v_flt);
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
        mpf_sub(result->result.v_flt, op1->result.v_flt, op2->result.v_flt);
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
        mpf_mul(result->result.v_flt, op1->result.v_flt, op2->result.v_flt);
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
        mpf_div(result->result.v_flt, op1->result.v_flt, op2->result.v_flt);
    }
}

// this function does NOT check whether both `op1` and `op2` are integers
void sevaluator_result_mod(FullResult *result, FullResult *op1, FullResult *op2) {
    sevaluator_result_init(result, R_INT);
    mpz_mod(result->result.v_int, op1->result.v_int, op2->result.v_int);
}

// this function does NOT check whether `op1` is negative
void sevaluator_result_sqrt(FullResult *result, FullResult *op1) {
    sevaluator_result_upgrade(op1, R_FLT);
    mpf_sqrt(result->result.v_flt, op1->result.v_flt);
}