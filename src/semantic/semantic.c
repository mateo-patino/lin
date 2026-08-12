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


bool is_scalar_mul_overflow(scalar_t a, scalar_t b) {
    return isinf(a * b);
}


bool is_scalar_div_overflow(scalar_t a, scalar_t b) {
    if (!b) {
        return false;
    }
    return isinf(a / b);
}


bool is_infinite_or_nan_scalar(scalar_t a) {
    return !isfinite(a);
}


bool has_finite_entries(const matrix_t *a) {
    if (!a) {
        return false;
    }   

    scalar_t *entry = a->data;
    for (unsigned int i = 0; i < a->nrow * a->ncol; i++) {
        if (!isfinite(entry[i])) {
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

        if (!sca || !lar) {
            return SEMANTIC_NULL_ARGS;
        }
        else if (is_infinite_or_nan_scalar(*sca) || is_infinite_or_nan_scalar(*lar)) {
            return SEMANTIC_INFINITE_OR_NAN_SCALAR;
        }
        else if (is_scalar_add_overflow(*sca, *lar)) {
            return SEMANTIC_FP_OVERFLOW;
        }

        return SEMANTIC_OK; 
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
            return SEMANTIC_INFINITE_OR_NAN_ENTRY;
        }

        return SEMANTIC_OK; 
    }
    
    return SEMANTIC_INCOMPATIBLE_OPERANDS;    
}


semantic_status valid_sub_operands(const token_t *a, const token_t *b) {
    /* 
    * Addition and subtraction have the same conditions to be valid operations. is_add_overflow and 
    * has_finite_entries use isinf, which checks for negative and positive infinity, so the entire 
    * valid_add_operands logic can be reused here. 
    */
    return valid_add_operands(a, b);
}


semantic_status valid_mul_operands(const token_t *first, const token_t *second) {
    if (!first || !second) { 
        return SEMANTIC_NULL_ARGS;
    }

    if (first->type == SCALAR && second->type == SCALAR) { 
        const scalar_t *sca = (const scalar_t *)first->obj;
        const scalar_t *lar = (const scalar_t *)second->obj;

        if (!sca || !lar) {
            return SEMANTIC_NULL_ARGS;
        }
        else if (is_infinite_or_nan_scalar(*sca) || is_infinite_or_nan_scalar(*lar)) {
            return SEMANTIC_INFINITE_OR_NAN_SCALAR;
        }
        else if (is_scalar_mul_overflow(*sca, *lar)) {
            return SEMANTIC_FP_OVERFLOW;
        }

        return SEMANTIC_OK;
    }
    else if (first->type == MATRIX && second->type == MATRIX) {
        const matrix_t *mat = (const matrix_t *)first->obj;
        const matrix_t *rix = (const matrix_t *)second->obj;

        if (!mat || !rix) {
            return SEMANTIC_NULL_ARGS;
        }
        /*
        * Like in addition, below we ONLY check that entries are finite. Matrix multiplication
        * can still produce overflow, but we let the algebra library detect that error so as 
        * avoid any long matrix math in this semantics layer.
        */
        else if (!has_finite_entries(mat) || !has_finite_entries(rix)) {
            return SEMANTIC_INFINITE_OR_NAN_ENTRY;
        }
        /*
        * NOTE: this function assumes the order of matrix multiplication is mat * rix.
        * Hence, we check that the number of columns in mat equals the number of rows in 
        * rix.
        */
        else if (mat->ncol != rix->nrow) {
            return SEMANTIC_INCOMPATIBLE_DIMENSIONS;
        }

        return SEMANTIC_OK;
    }

    return SEMANTIC_INCOMPATIBLE_OPERANDS;
}


semantic_status valid_div_operands(const token_t *a, const token_t *b) {
    if (!a || !b) {
        return SEMANTIC_NULL_ARGS;
    }

    if (a->type != SCALAR || b->type != SCALAR) {
        return SEMANTIC_INCOMPATIBLE_OPERANDS;
    }

    const scalar_t *sca = (const scalar_t *)a->obj;
    const scalar_t *lar = (const scalar_t *)b->obj;

    if (!sca || !lar) {
        return SEMANTIC_NULL_ARGS;
    }
    else if (is_infinite_or_nan_scalar(*sca) || is_infinite_or_nan_scalar(*lar)) {
        return SEMANTIC_INFINITE_OR_NAN_SCALAR;  
    }
    /*
     * NOTE: this function assumes that the order of divison is sca / lar.
     */
    else if (is_scalar_div_overflow(*sca, *lar)) {
        return SEMANTIC_FP_OVERFLOW;
    }
    /* TOOD: hanlde division by zero! */
    else if (is_division_by_zero_scalar())

    return SEMANTIC_OK;
} 


semantic_status valid_det_operand(const token_t *a) {
    if (!a) {
        return SEMANTIC_NULL_ARGS;
    } 

    if (a->type != MATRIX) {
        return SEMANTIC_EXPECTED_MATRIX;
    }

    const matrix_t *mat = (const matrix_t *)a->obj;

    if (!mat) {
        return SEMANTIC_NULL_ARGS;
    }
    else if (!has_finite_entries(mat)) {
        return SEMANTIC_INFINITE_OR_NAN_ENTRY;
    }
    /* Determinants are only defined for square matrices */
    else if (mat->ncol != mat->nrow) {
        return SEMANTIC_NONSQUARE_MATRIX;
    }

    return SEMANTIC_OK;
}


semantic_status valid_rref_operand(const token_t *a) {
    if (!a) {
        return SEMANTIC_NULL_ARGS;
    }

    if (a->type != MATRIX) {
        return SEMANTIC_EXPECTED_MATRIX;
    }

    const matrix_t *mat = (const matrix_t *)a->obj;

    if (!mat) {
        return SEMANTIC_NULL_ARGS;
    }

    return has_finite_entries(mat) ? SEMANTIC_OK : SEMANTIC_INFINITE_OR_NAN_ENTRY;
}


semantic_status valid_inv_operand(const token_t *a) {
    if (!a) {
        return SEMANTIC_NULL_ARGS;
    }

    if (a->type != MATRIX) {
        return SEMANTIC_EXPECTED_MATRIX;
    }
   
    const matrix_t *mat = (const matrix_t *)a->obj;
    if (!mat) {
        return SEMANTIC_NULL_ARGS;
    }

    /*
    * Only square matrices can be inverted. Generalized (one-sided) inverses that arise
    * from inverting a non-square matrix are not currently supported.
    */
    if (mat->ncol != mat->nrow) {
        return SEMANTIC_NONSQUARE_MATRIX;
    }
    else if (!has_finite_entries(mat)) {
        return SEMANTIC_INFINITE_OR_NAN_ENTRY;
    }

    /* It is tempting to check the determinant of the matrix, but this implies calling
    * the algebra library, and this module shall only perform high-level checks and avoid
    * any heavy math. If the matrix is not invertible, the algebra functions will detect it
    * when their time comes */

    return SEMANTIC_OK;
}
