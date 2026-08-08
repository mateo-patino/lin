#include "ast.h"


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


node_t *initialize_node(const token_t *token, node_t *left, node_t *right) {
    node_t *node = malloc(sizeof(node_t));
    if (!node) {
        return NULL;
    }
    node->token = token;
    node->left = left;
    node->right = right;

    return node;
}
