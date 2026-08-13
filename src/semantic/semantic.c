#include "semantic.h"
#include "types/token.h"
#include "types/matrix.h"
#include "errorprinter.h"

#include <assert.h>
#include <math.h>


/*
* An internal interface for setting a semantic status. Used during recursion by 
* is_semantically_valid_ast
*/
static semantic_status internal_semantic_status = SEMANTIC_OK;
static bool has_error_status = false; /* SEMANTIC_OK is the only NON-error status */

static void set_status(semantic_status status) {
    if (has_error_status) {
        return;
    }
    internal_semantic_status = status;
    if (status != SEMANTIC_OK) {
        has_error_status = true;
    }
}


/* This is the only function in this module (and its helper) that shall set 
* errors to the global buffer. It will do so via set_type_error and set_operand_error. */
semantic_status is_semantically_valid_ast(const ast_t *ast) {
    if (!ast || !ast->root) {
        return SEMANTIC_NULL_ARGS;
    }
    
    set_status(SEMANTIC_OK); 

 

    return SEMANTIC_OK;
}


/* This function is a helper to set_type_error */
static const char *op_to_str(operator_type op) {
    switch (op) {
        case ADD:
            return "addition";
        case SUB:
            return "subtraction";
        case MUL:
            return "multiplication";
        case DIV:
            return "division";
        case DET:
            return "determinant";
        case RREF:
            return "RREF";
        case INV:
            return "inversion";
        case NUM_OP:
        default:
            return "?";
    }
}


/*
* Writes an error message to the global buffer concerning a type error.
*/
static void set_type_error(operator_type op, io_type left, io_type right) {
    bool unary_type_error = false;
    const char *op_str = op_to_str(op);
    const char *left_str, *right_str;

    /* Note we assume the convention that unary operators have a NULL left child and
    * non-NULL right child */
    if (left != SEM_NULL) {
        left_str = left == SEM_SCALAR ? "scalar" : "matrix";
    }
    else {
        unary_type_error = true;
        assert(right != SEM_NULL);
    }
     
    right_str = right == SEM_SCALAR ? "scalar" : "matrix";

    if (unary_type_error) {
        set_error("Operand of type '%s' is mathematically incompatible with '%s'.",
                   right_str, op_str);
    }
    else {
        set_error("Operands of type '%s' and '%s' are mathematically incompatible with %s.",
                  left_str, right_str, op_str); 
    }
}


/*
* Helper to is_valid_ast_helper. It takes an operator type and two operand types
* and returns the expected output type of the operator if it were to operate
* on the two operand types.
*
* It returns SEM_SCALAR or SEM_MATRIX upon success and SEM_NULL upon failure.
*/
static io_type get_output_type(operator_type op, io_type left, io_type right) {
    switch (op) {

        case ADD:
        case SUB:
            if (left == SEM_SCALAR && right == SEM_SCALAR) {
                return SEM_SCALAR;
            }
            else if (left == SEM_MATRIX && right == SEM_MATRIX) {
                return SEM_MATRIX;
            }
            return SEM_NULL;

        case MUL:
            if (left == SEM_SCALAR && right == SEM_SCALAR) {
                return SEM_SCALAR;
            }
            else if (left == SEM_MATRIX && right == SEM_MATRIX) {
                return SEM_MATRIX;
            }
            else if ((left == SEM_SCALAR && right == SEM_MATRIX) || 
                    (left == SEM_MATRIX && right == SEM_SCALAR)) {
                return SEM_MATRIX;
            }
            return SEM_NULL;

        case DIV:
            if (left == SEM_SCALAR && right == SEM_SCALAR) {
                return SEM_SCALAR;
            }
            return SEM_NULL;

        case DET:
        case RREF:
        case INV:
            /* Note we use the convention that unary operators have NULL left children
            * and non-NULL right children. */
            if (left == SEM_NULL && right == SEM_MATRIX) {
                return SEM_MATRIX;
            }
            return SEM_NULL;

        case NUM_OP:
        default:
            return SEM_NULL;
    }
}


static io_type is_valid_ast_helper(const node_t *node) {
    if (!node) {
        return SEM_NULL;
    }

    const token_t *token = node->token;
    assert(token != NULL && token->obj != NULL);
    if (token->type == SCALAR) {
        return SEM_SCALAR;
    }
    else if (token->type == MATRIX) {
        return SEM_MATRIX;
    }

    /* If node is not a scalar or a matrix, it must be an operator, so recurse */ 
    assert(token->type == OPERATOR);
    io_type left_type = is_valid_ast_helper(node->left);
    io_type right_type = is_valid_ast_helper(node->right);

    /* Check left and right types match the expected input types. Write to
    * global error buffer if not. */
    operator_type op = *(operator_type *)token->obj;
    if (!are_valid_input_types(op, left_type, right_type)) {
        set_type_error(op, left_type, right_type);
    }

    /* Regardless of a type error, we continue. The first type error written to the global
    * buffer is kept and not overwritten, so we just proceed until finishing the entire traversal */
    return get_output_type(op, left_type, right_type); 
}




bool are_valid_input_types(operator_type op, io_type left, io_type right) {
    switch (op) {

        /* Add, subtract, and multiply can be done with scalars and matrices */
        case ADD:
        case SUB:
            return (left == SEM_SCALAR && right == SEM_SCALAR) ||
                   (left == SEM_MATRIX && right == SEM_MATRIX);
        case MUL:
            return (left == right && left == SEM_SCALAR) ||
                   (left == right && left == SEM_MATRIX) ||
                   ((left == SEM_SCALAR && right == SEM_MATRIX) ||
                    (left == SEM_MATRIX && right == SEM_SCALAR));

        /* Division can only be done with scalars */
        case DIV:
            return left == SEM_SCALAR && right == SEM_SCALAR;

        /* Determinants, rref, and inverses can only be done with matrices */
        case DET:
        case RREF:
        case INV:
            return left == SEM_NULL && right == SEM_MATRIX;

        /* These shouldn't happen */
        case NUM_OP:
        default:
            return false;
    }
}


bool set_semantic_error(semantic_status status) {
    switch (status) {
        case SEMANTIC_OK:
            return false;
        case SEMANTIC_INCOMPATIBLE_OPERANDS:
            return set_error("Operands are mathematically incompatible.");
        case SEMANTIC_FP_OVERFLOW:
            return set_error("Foating-point overflow.");
        case SEMANTIC_INCOMPATIBLE_DIMENSIONS:
            return set_error("Incompatible matrix dimensions.");
        case SEMANTIC_INFINITE_OR_NAN_SCALAR:
            return set_error("Infinite or undefined scalar.");
        case SEMANTIC_INFINITE_OR_NAN_ENTRY:
            return set_error("Infinite or undefined matrix entry.");
        case SEMANTIC_DIVISION_BY_ZERO:
            return set_error("Division by zero.");
        case SEMANTIC_NONSQUARE_MATRIX:
            return set_error("Expected an NxN (square) matrix, got non-square matrix.");
        case SEMANTIC_EXPECTED_MATRIX:
            return set_error("Expected a matrix operand.");
        case SEMANTIC_NULL_ARGS:
            return set_error("Expected dereferenceable pointer, got NULL.");
        default:
            return false;
    }
}


bool is_scalar_add_overflow(scalar_t a, scalar_t b) {
    return isinf(a + b);
}


bool is_scalar_mul_overflow(scalar_t a, scalar_t b) {
    return isinf(a * b);
}


/* Does not catch NaNs, only overflows (infinities) */
bool is_scalar_div_overflow(scalar_t a, scalar_t b) {
    return isinf(a / b);
}


/* Does catch NaNs and infinities, which are both ways different systems
* represent division by zero */
bool is_division_by_zero(scalar_t a, scalar_t b) {
    return !isfinite(a / b);
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


/*
* FIX: we should support a scalar times a matrix. Currently only scalar-scalar and matrix-matrix 
* is allowed.
*/

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
     * Below we check that sca / lar does not produce positive or negative infinity.
     */
    else if (is_scalar_div_overflow(*sca, *lar)) {
        return SEMANTIC_FP_OVERFLOW;
    }
    /* 
    * Below we check that sca / lar does not produce infinity or NaN, which are both
    * ways in which floating-point division-by-zero is represented in different systems.
    */
    else if (is_division_by_zero(*sca, *lar)) {
        return SEMANTIC_DIVISION_BY_ZERO;
    }

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
