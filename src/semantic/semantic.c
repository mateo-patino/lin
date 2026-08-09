#include "semantic.h"
#include "types/matrix.h"

#include <math.h>


semantic_status is_semantically_valid_ast(const ast_t *ast) {
    /* TODO */
    return SEMANTIC_OK;
}


bool is_scalar_add_overflow(scalar_t a, scalar_t b) {
    return isinf(a + b);
}


bool has_finite_entries(const matrix_t *a) {
    if (!a) {
        return false;
    }   

    scalar_t *entry = a->data;
    for (unsigned int i = 0; i < a->nrow * a->ncol; i++) {
        if (isinf(entry[i])) {
            return false;
        }
    }
    return true;
}


semantic_status valid_add_operands(const token_t *a, const token_t *b) {
    if (!a || !b) {
        return SEMANTIC_NULL_ARGS;
    }

    if (a->type == SCALAR && b->type == SCALAR) {
        const scalar_t *sca = (const scalar_t *)a->obj;
        const scalar_t *lar = (const scalar_t *)b->obj;
        return (sca && lar && is_scalar_add_overflow(*sca, *lar)) ? SEMANTIC_FP_OVERFLOW : SEMANTIC_OK; 
    }
    else if (a->type == MATRIX && b->type == MATRIX) {
        const matrix_t *mat = (const matrix_t *)a->obj;
        const matrix_t *rix = (const matrix_t *)b->obj;

        if (!mat || !rix) {
            return SEMANTIC_NULL_ARGS;
        }
        else if (!have_equal_dimensions(mat, rix)) {
            return SEMANTIC_INCOMPATIBLE_DIMENSIONS;
        }
        /*
         * Below we check that each entry in the matrices is not infinite. Note that entry-wise addition
         * can result in overflow even if all entries are finite, but this overflow is detected inside 
         * the algebra module during calculation.
         */
        else if (!has_finite_entries(mat) || !has_finite_entries(rix)) {
            return SEMANTIC_INFINITE_ENTRY;
        }

        return SEMANTIC_OK; 
    }
    
    return SEMANTIC_INCOMPATIBLE_OPERANDS;    
}


semantic_status valid_sub_operands(const token_t *a, const token_t *b) {
    /* Addition and subtraction have the same conditions to be valid operations. is_add_overflow
    * uses isinf, which checks for negative and positive infinity, so the entire valid_add_operands
    * logic can be reused here. */
    return valid_add_operands(a, b) ;
}


semantic_status valid_mul_operands(const token_t *a, const token_t *b) {
    if (!a || !b) { 
        return SEMANTIC_NULL_ARGS;
    }

    if (a->type == SCALAR && b->type == SCALAR) {
         
    }
}


semantic_status valid_div_operands(const token_t *a, const token_t *b) {
    if (!a || !b) {
        return SEMANTIC_NULL_ARGS;
    }
    /* TODO: check for NaN */
    return a->type == SCALAR && b->type == SCALAR ? SEMANTIC_OK : SEMANTIC_INCOMPATIBLE_OPERANDS;
} 
