#ifndef SEMANTIC_H
#define SEMANTIC_H

/*
* This module provides functions to perform semantic checks on
* different linear algebra operations and full abstract syntax trees.
* The functions mostly expect token_t structs.
*
* Here "semantic" refers to mathematical conditions that must be met for 
* an operation to be sensible. For example, checking that the matrix operand
* for an INV (inverse) operation is square or that the operands for a ADD
* operation have the same type and dimensions.
*
* A set of enum codes is provided to describe in detail what kind of semantic
* error was found.
*
* In general, functions here do not use the global errorprinter module. Setting
* error messages is left to the caller who should have access to inputs and the 
* semantic status returned by the functions here.
*/

#include "parser/ast.h"
#include "types/token.h"

#include <stdbool.h>


/* Error codes */
typedef enum {

    /* Operands are valid and can be fed into some operation */
    SEMANTIC_OK,

    /* Operands are logically incompatible with an operation or with each other */
    SEMANTIC_INCOMPATIBLE_OPERANDS,

    /* Operation results in floating-point positive or minus infinity. This differs from 
    * SEMANTIC_INFINITE_* in that an operation with VALID operands produced the overflow */
    SEMANTIC_FP_OVERFLOW,

    /* Two matrices do not have the same or coherent dimensions and hence cannot be operated upon */
    SEMANTIC_INCOMPATIBLE_DIMENSIONS,

    /* A scalar produced isinf(scalar) == true */
    SEMANTIC_INFINITE_OR_NAN_SCALAR,

    /* A matrix entry produced isinf(entry) ==  true */
    SEMANTIC_INFINITE_OR_NAN_ENTRY,

    /* Attempt to divide by zero */
    SEMANTIC_DIVISION_BY_ZERO,

    /* Got a non-square matrix when a square matrix was needed */
    SEMANTIC_NONSQUARE_MATRIX,

    /* An operation expected a matrix but got a different token or object */
    SEMANTIC_EXPECTED_MATRIX,

    /* A pointer to token or obj is NULL */
    SEMANTIC_NULL_ARGS
} semantic_status;


/*
* Returns SEMANTIC_OK if `ast` is a semantically valid AST.
*
* TODO: define what "being semantically valid" implies.
*/
semantic_status is_semantically_valid_ast(const ast_t *ast);


/*
* Returns true if the floating-point addition `a` + `b` gives either 
* +infinity or -infinity (i.e. floating-point overflow).
*
* Note that floating-point overflow is also defined to account for 
* a large negative value.
*
* In this and the subsequent overflow functions, we use isinf to strictly
* catch overflows and ignore NaNs. No NaNs should really ever be passed here
* but note that this a limitation of these is_*_overflow functions.
*/
bool is_scalar_add_overflow(scalar_t a, scalar_t b);


/*
* Returns true if `a * b` results in floating-point overflow.
*/
bool is_scalar_mul_overflow(scalar_t a, scalar_t b);

/*
* Returns true if `a / b` results in floating-point overflow.
* This function uses isinf(), which checks for positive or negative
* infinity but ignored NaNs.
*
* The alternative to this function is `is_division_by_zero` which uses
* isfinite to check for NaN and infinity. However, using `is_division_by_zero`,
* the caller doesn't know if overflow (infinity) or NaN occurred. The purpose
* of is_scalar_div_overflow is to catch overflows (infinities) only, while
* `is_division_by_zero` catches both overflows and NaNs because some systems
* represent floating-point division-by-zero as either NaN or infinity.
*/
bool is_scalar_div_overflow(scalar_t a, scalar_t b);


/*
* Returns true if `a / b` produces a NaN or positive or negative infinity.
*
* This function is able to catch floating-point overflows (infinity) and 
* NaNs because different systems represent floating-point division-by-zero
* in either way.
*/
bool is_division_by_zero(scalar_t a, scalar_t b);


/*
* Returns isfinite(a) (if `a` is positive or negative infinity or NaN)
*/
bool is_infinite_or_nan_scalar(scalar_t a);


/*
* Returns true if all entries in matrix `a` are finite according to
* !isfinite().
*
* Infinity and NaNs are rejected.
*/
bool has_finite_entries(const matrix_t *a);

/*
* Returns SEMANTIC_OK if `a` and `b` are valid operands for addition.
*
* Linear algebra does not define addition between a scalar and a matrix,
* so only scalar-with-scalar and matrix-with-matrix addition is allowed.
*/
semantic_status valid_add_operands(const token_t *a, const token_t *b);

/*
* Returns SEMANTIC_OK if `a` and `b` are valid operands for substraction
*
* Like with addition, only scalar-with-scalar and matrix-with-matrix subtraction
* is allowed.
*/
semantic_status valid_sub_operands(const token_t *a, const token_t *b);


/*
* Returns SEMANTIC_OK if `first` and `second` are valid multiplication operands
* in the order first * second. 
*
* Recall matrix multiplication is not commutative, so if `first` and `second` are
* matrices, this function assumes `first` is the left-side operand and `second` is
* the right-side operand, and it returns true if matrix multiplication can be
* performed in this order.
*
* Scalars are also supported in multiplication.
*/
semantic_status valid_mul_operands(const token_t *a, const token_t *b);

/*
* Returns SEMANTIC_OK if `a` and `b` are valid operands for division.
* Matrix divison is not a thing, so `a` and `b` must be scalars.
*
* The function assumes that the order of divison is a / b.
*/
semantic_status valid_div_operands(const token_t *a, const token_t *b);


/*
* Returns SEMANTIC_OK if it is possible to compute the detemrinant of `a`
*/
semantic_status valid_det_operand(const token_t *a);


/*
* Returns SEMANTIC_OK if it is possible to perform row-reduction on `a`.
*
* Row-reduction is a very common operation with relaxed requirements. As long as
* all entries are finite (i.e. isfinite returns true, non-NaN and non-infinite),
* SEMANTIC_OK is returned.
*/
semantic_status valid_rref_operand(const token_t *a);

/*
* Returns SEMANTIC_OK if `a` is a matrix with 1) square dimensions, 2) finite
* entries
*
* Note: only square matrices are valid INV operands. Generalized or one-sided inverses
* are NOT currently supported. Only ordinary square inverses are allowed.
*/
semantic_status valid_inv_operand(const token_t *a);

#endif
