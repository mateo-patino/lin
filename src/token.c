#include "token.h"

/* Table of operator arities */
const char arity[NUM_OP] = {
    [MAT_ADD] = 2,
    [MAT_SUB] = 2,
    [MAT_MUL] = 2,
    [SMUL] = 2,
    [DET] = 1,
    [RREF] = 1,
    [INV] = 1
};

/* Table of precedences */
const char precedence[NUM_OP] = {
    [MAT_ADD] = 0,
    [MAT_SUB] = 0,
    [MAT_MUL] = 1,
    [SMUL] = 1,
    [DET] = 2,
    [RREF] = 2,
    [INV] = 2
};


const assoc associativity[NUM_OP] = {
    [MAT_ADD] = LEFT_ASSOC,
    [MAT_SUB] = LEFT_ASSOC,
    [MAT_MUL] = LEFT_ASSOC,
    [SMUL] = LEFT_ASSOC,
    [DET] = RIGHT_ASSOC,
    [RREF] = RIGHT_ASSOC,
    [INV] = RIGHT_ASSOC
};
