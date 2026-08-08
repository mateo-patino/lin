#include "semantic.h"
#include "types/matrix.h"
#include "errorprinter.h"


semantic_status is_semantically_valid_ast(const ast_t *ast) {
    /* TODO */
    return SEMANTIC_OK;
}



semantic_status valid_add_operands(const token_t *a, const token_t *b) {
    if (!a || !b) {
        return SEMANTIC_NULL_ARGS;
    }

    if (a->type == SCALAR && b->type == SCALAR) {
        return SEMANTIC_OK; 
    }
    else if (a->type == MATRIX && b->type == MATRIX) {
        const matrix_t *mat = (const matrix_t *)a->obj;
        const matrix_t *rix = (const matrix_t *)b->obj;

        return have_equal_dimensions(mat, rix) ? SEMANTIC_OK : SEMANTIC_UNEQUAL_DIMENSIONS;
    }
    
    return SEMANTIC_INCOMPATIBLE_OPERANDS;    
}
