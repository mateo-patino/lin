#ifndef PARSER_H
#define PARSER_H

/* Interface for the parser module. 
*
* The parser relies on an Abstract Syntax Tree, which is constructed from a
* tokens array. This interface provides both an AST constructor and evaluator.
*/

#include "token.h"

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

#endif
