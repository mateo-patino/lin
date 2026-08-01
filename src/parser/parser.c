#include "parser.h"

#include <stdlib.h>


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

    node_t *root = create_ast_helper(tokens, sz, status);
    if (!root || get_status(status) != AST_OK) {
        fully_free_ast(tree);
        return NULL;
    }

    return tree;
    
}


node_t *create_ast_helper(const token_t *token, size_t sz, ast_status *status) {
    /* TODO */
    return NULL;
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

