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
    SEMANTIC_OK,
    SEMANTIC_INCOMPATIBLE_OPERANDS,
    SEMANTIC_FP_OVERFLOW,
    SEMANTIC_UNEQUAL_DIMENSIONS,
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
*/
bool add_overflow(scalar_t a, scalar_t b);

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
* Returns SEMANTIC_OK if `a` and `b` are valid multiplication operands.
*/
semantic_status valid_mul_operands(const token_t *a, const token_t *b);

/*
* Returns SEMANTIC_OK if `a` and `b` are valid operands for division.
* Matrix divison is not a thing, so `a` and `b` must be scalars.
*/
semantic_status valid_div_operands(const token_t *a, const token_t *b);


#endif
