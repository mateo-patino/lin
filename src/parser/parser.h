#ifndef PARSER_H
#define PARSER_H

/* Interface for the parser module. 
*
* The parser relies on an Abstract Syntax Tree, which is constructed from a
* tokens array. This interface provides both an AST constructor and evaluator.
*/

#include "types/token.h"

/*
* AST node type. Nodes are simply wrappers around a token_t pointer which points
* to the tokens array in memory.
*/
typedef struct node_t{
    const token_t *token;
    struct node_t *left;
    struct node_t *right;
} node_t;


/* AST type */
typedef struct {
    node_t *root;
} ast_t;


/*
* Error interface for AST building and creation.
*/
typedef enum {
    AST_OK,
    AST_MEMORY_FAILURE,
    AST_INVALID_TOKENS,
    AST_STATUS_DNE
} ast_status;


/*
* Creates an abstract syntax tree given a tokens array `tokens` of `sz` 
* token_t structs.
*
* Returns a pointer to an heap-allocated ast_t where ast_t.root points to a node_t
* struct representing the root of the tree.
*
* If the tokens cannot be converted into an AST or a memory failure occurs, NULL is 
* returned and `st` is set to indicate the error. In this case no heap memory needs
* to be freed by the caller; all partially allocated nodes are freed internally.
*/
ast_t *create_ast_from_tokens(const token_t *tokens, size_t sz, ast_status *status);


/*
* Recursively creates an AST from a token_t array `tokens` of `sz` elements. 
* 
* Returns a pointer to the root of the tree upon success. If a failure occurs,
* an error code is written to `status` and a pointer to the root of the tree 
* is returned so the caller can free the partially-built tree (unless the root
* could not be initialized. This is the only case where NULL is returned).
*/
node_t *create_ast_helper(const token_t *token, size_t sz, ast_status *status);


/*
* Recursively frees an AST with root `ast->root` and also `ast` itself.
*/
void fully_free_ast(ast_t *ast);


/*
* Recursively frees a subtree rooted at `node`. Note that the token_t pointed
* at by `node->token` is NOT freed.
*/
void free_subtree(node_t *node);


#endif
