#include "token.h"
#include "matrix.h"

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


/* Operator aliases. Note each array of aliases is NULL terminated. */
static const char *mat_add_alias[] = { "add", "plus", "+", NULL };
static const char *mat_sub_alias[] = { "sub", "minus", "-", NULL };
static const char *mat_mul_alias[] = { "mul", "times", "*", NULL };
static const char *smul_alias[] = { "smul", "scale", "s*", NULL };
static const char *det_alias[] = { "det", "determinant", "detof", NULL };
static const char *rref_alias[] = { "rref", "reduced", NULL };
static const char *inv_alias[] = { "inv", "inverse", NULL };

const char **operator_alias[NUM_OP] = {
    [MAT_ADD] = mat_add_alias,
    [MAT_SUB] = mat_sub_alias,
    [MAT_MUL] = mat_mul_alias,
    [SMUL] = smul_alias,
    [DET] = det_alias,
    [RREF] = rref_alias,
    [INV] = inv_alias,
};


void fully_free_tokens(token_t *tokens, size_t count) {
    free_token_objs_by_count(tokens, count);
    free(tokens);
}


void free_token_objs_by_count(token_t *tokens, size_t count) {
    if (!tokens) {
        return;
    }
    for (size_t i = 0; i < count; i++) {
        free_token_obj(tokens + i);
    }
}


void free_token_obj(token_t *tok) {
    if (!tok || !tok->obj) {
        return;
    }
    
    if (tok->type == MATRIX) {
        free_matrix((matrix_t *)tok->obj);
    }
    else {
        free(tok->obj);
    }
}
