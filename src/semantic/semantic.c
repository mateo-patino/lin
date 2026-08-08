#include "semantic.h"
#include "types/matrix.h"

#include <math.h>


semantic_status is_semantically_valid_ast(const ast_t *ast) {
    /* TODO */
    return SEMANTIC_OK;
}


bool add_overflow(scalar_t a, scalar_t b) {
    return isinf(a + b);
}


semantic_status valid_add_operands(const token_t *a, const token_t *b) {
    if (!a || !b) {
        return SEMANTIC_NULL_ARGS;
    }

    if (a->type == SCALAR && b->type == SCALAR) {
        const scalar_t *sca = (const scalar_t *)a->obj;
        const scalar_t *lar = (const scalar_t *)b->obj;
        return add_overflow(*sca, *lar) ? SEMANTIC_FP_OVERFLOW : SEMANTIC_OK; 
    }
    else if (a->type == MATRIX && b->type == MATRIX) {
        const matrix_t *mat = (const matrix_t *)a->obj;
        const matrix_t *rix = (const matrix_t *)b->obj;

        return have_equal_dimensions(mat, rix) ? SEMANTIC_OK : SEMANTIC_UNEQUAL_DIMENSIONS;
    }
    
    return SEMANTIC_INCOMPATIBLE_OPERANDS;    
}


semantic_status valid_sub_operands(const token_t *a, const token_t *b) {
    /* Addition and subtraction have the same conditions to be valid operations */
    return valid_add_operands(a, b) ;
}


semantic_status valid_mul_operands(const token_t *a, const token_t *b) {
    /* TODO */
}


semantic_status valid_div_operands(const token_t *a, const token_t *b) {
    if (!a || !b) {
        return SEMANTIC_NULL_ARGS;
    }
    /* TODO: check for NaN */
    return a->type == SCALAR && b->type == SCALAR ? SEMANTIC_OK : SEMANTIC_INCOMPATIBLE_OPERANDS;
}
