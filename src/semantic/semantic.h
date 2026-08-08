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
*/

#include "parser/ast.h"
#include "types/token.h"

#include <stdbool.h>



/* Error codes */
typedef enum {
    SEMANTIC_OK,
    SEMANTIC_INCOMPATIBLE_OPERANDS,
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
* Returns SEMANTIC_OK if `a` and `b` are valid operands for addition.
*
* Linear algebra does not define addition between a scalar and a matrix,
* so only scalar-with-scalar and matrix-with-matrix addition is allowed.
*/
semantic_status valid_add_operands(const token_t *a, const token_t *b);


#endif
