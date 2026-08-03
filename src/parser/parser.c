#include "parser.h"
#include "types/token.h"
#include "errorprinter.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>


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
            goto UPDATE_LAST_SO_FAR;
        }
        else if (other->prec == last_so_far->prec) {

            /* 
            * If precedences are equal, then choose the operator that would be
            * evaluated last according to associativity and index. A key invariant: if two
            * operators have the same precedence, they must have the same associativity
            * (any correctly defined precedence table must satisfy this). Therefore,
            * `other` and `last_so_far` must have the same associativity. If they're
            * left-associative, pick the operator with the greatest `index` (furthest to
            * the right); if they're right-associative, pick the operator furthest to the 
            * left).
            */

            if (other->assoc == LEFT_ASSOC) { /* Could've checked last_so_far->assoc too */
                if (other->index > last_so_far->index) {
                    goto UPDATE_LAST_SO_FAR;
                }
            }
            else if (other->assoc == RIGHT_ASSOC){
                if (other->index < last_so_far->index) {
                    goto UPDATE_LAST_SO_FAR;
                }
            }

        }
    }

UPDATE_LAST_SO_FAR:
    memcpy(last_so_far, other, sizeof(tuple_t));
}


/*
* Returns the index of the last operation that would occur according to the
* precedence and associativity rules in types/token.h should one evaluate the
* tokens expression in the range [low, high] exclusive.
*
* If no operator token is found in the [low, high], -1 is returned.
*/
static int find_last_op_index(const token_t *tokens, int low, int high) {
    
    /* Some starting values that no valid operator could have */
    tuple_t last_so_far = {NUM_OP, LONG_MAX, LONG_MAX, -1, -1};
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

    return last_so_far.index;
}


/* Returns true if `token` is a matrix, scalar, or another operand type. */
static bool is_operand_type(const token_t *token) {
    if (!token) {
        return false;
    }
    return token->type == SCALAR || token->type == MATRIX;
}


/*
* Returns the index inside of the range [low, high] of exactly one operand type.
* If no operands exist or more than one does, -1 is returned.
*/
static int get_remaining_operand(const token_t *tokens, int low, int high) {
    int index = -1;
    for (int i = low; i <= high; i++) {
        if (is_operand_type(tokens + i)) {
            /* `index` must be -1 before setting it to a valid index value. *
               If `index` has been previously set and we attempt to set it again,
               more than one operand token must exist, so we return in failure. */
            if (index != -1) {
                return -1;
            }
            index = i;
        }
    }
    return index;
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

    node_t *new_node;
    node_t *left;
    node_t *right;

    int last_op_index = find_last_op_index(tokens, low, high);

    /* No operator token found in [low, high]. Exactly ONE operand must exist */
    if (last_op_index == -1) {
        int remaining_operand_index;

        /* -1 means not EXACTLY ONE operand was found. `tokens` is a malformed expression. */
        if ((remaining_operand_index = get_remaining_operand(tokens, low, high)) == -1) {
            set_error("Invalid algebraic expression.");
            RETURN_NULL_AND_STATUS(status, AST_INVALID_EXPRESSION);
        }

        /* We have one operand, so recursion stops. Initialize node and return it. */
        new_node = initialize_node(tokens + remaining_operand_index, NULL, NULL);
        if (!new_node) {
            set_error("malloc() failed.");
            RETURN_NULL_AND_STATUS(status, AST_MEMORY_FAILURE);
        }
        
        return new_node;
    }

    /* An operator token was found, so recurse on the left and right sub-expressions */
    left = create_ast_helper(tokens, low, last_op_index - 1, status);
    right = create_ast_helper(tokens, last_op_index + 1, high, status);

    new_node = initialize_node(tokens + last_op_index, left, right);
    if (!new_node) {
        set_error("malloc() failed.");
        
        /* Left and right subtrees could have been allocated so free */
        free_subtree(left);
        free_subtree(right);

        RETURN_NULL_AND_STATUS(status, AST_MEMORY_FAILURE);
    }
 
    return new_node;
}

