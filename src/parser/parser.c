#include "parser.h"
#include "types/token.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>


/* Setting an error status and returning NULL is a commnon pattern */
#define RETURN_NULL_AND_STATUS(status, val)  \
    do { \
        set_status(status, val); \
        return NULL; \
    } while (0)


static void set_status(ast_status *status, ast_status val) {
    /* If *status != AST_OK (error already occurred), do not overwrite */
    if (!status || *status != AST_OK) {
        return;
    }
    *status = val;
}

static ast_status get_status(ast_status *status) {
    return status ? *status : AST_STATUS_DNE;
}


ast_t *create_ast_from_tokens(const token_t *tokens, size_t sz, ast_status *status) {
    if (!tokens || !sz) {
        RETURN_NULL_AND_STATUS(status, AST_INVALID_TOKENS);
    }

    /* Set global status to OK before starting */
    set_status(status, AST_OK);
    
    ast_t *tree;
    if (!(tree = malloc(sizeof(ast_t)))) {
        RETURN_NULL_AND_STATUS(status, AST_MEMORY_FAILURE);
    }

    node_t *root = create_ast_helper(tokens, 0, sz-1, status);
    if (!root || get_status(status) != AST_OK) {
        fully_free_ast(tree);
        return NULL;
    }

    return tree;
}


node_t *create_ast_helper(const token_t *tokens, int low, int high, ast_status *status) {
    if (!tokens) {
        return NULL;
    }
    int last_op_index = find_last_op_index(tokens, low, high)
    return NULL;
}


/*
* Helper to find_last_op_index function (see below).
*
* Populate the tuple `current_op` with depth, prec, assoc, and 
* index values.
*/
static void set_operator_tuple(tuple_t *op_tuple, operator_type op_type, int depth, int index) {
    if (!op_tuple) {
        return;
    }
    op_tuple->op_type = op_type;
    op_tuple->depth = depth;
    op_tuple->prec = precedence[op_type];
    op_tuple->assoc = associativity[op_type];
    op_tuple->index = index;
}


/*
* Helper to the find_last_op_index function (see below).
*
* The function below updates `last_so_far` by comparing it to `other` so 
* `last_so_far` contains the operator that would be evaluated last in
* an expression.
*/
static void compare_operators(tuple_t *other, tuple_t *last_so_far) {
    if (!other || !last_so_far) {
        return;
    }
    
    /* Always choose the operator with the lowest depth */
    if (other->depth < last_so_far->depth) {
        goto UPDATE_LAST_SO_FAR;
    }
    else if (other->depth == last_so_far->depth) {
        /* If depths are equal, choose op with least precedence */
        if (other->prec < last_so_far->prec) {
            /* TODO */
        }
    }

UPDATE_LAST_SO_FAR:
    memcpy(last_so_far, other, sizeof(tuple_t));
}


/*
* Returns the index of the last operation that would occur according to the
* precedence and associativity rules in types/token.h should one evaluate the
* tokens expression in the range [low, high] exclusive.
*/
static int find_last_op_index(const token_t *tokens, int low, int high) {
    
    tuple_t last_so_far = {LONG_MAX};
    tuple_t current_op;
    int current_depth = 0;
    token_type tok_type;
    operator_type op;

    for (int i = low; i < high; i++) {

        tok_type = tokens[i].type;

        switch (tok_type) {
            case SCALAR:
            case MATRIX:
            case TOKENS_END:
                continue;
            case LPAREN:
                current_depth++;
                continue;
            case RPAREN:
                current_depth--;
                continue;
            case OPERATOR:
                op = *(operator_type *)tokens[i].obj;
                set_operator_tuple(&current_op, op, current_depth, i);
                compare_operators(&current_op, &last_so_far);
        }
    }

    return last_so_far->index;
}


void free_subtree(node_t *node) {
    if (!node) {
        return;
    }
    free_subtree(node->left);
    free_subtree(node->right);
    free(node);
}


void fully_free_ast(ast_t *ast) {
    if (!ast || !ast->root) {
        return;
    }
    free_subtree(ast->root);
    free(ast);
}

