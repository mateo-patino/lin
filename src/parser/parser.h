#ifndef PARSER_H
#define PARSER_H

/* Interface for the parser module. 
*
* The parser relies on an Abstract Syntax Tree, which is constructed from a
* tokens array. This interface provides both an AST constructor and evaluator.
*/

#include "types/token.h"
#include "ast.h"

/* 
* A tuple used for storing an operator depth's, precedence, associativity, and 
* index within the tokens array. This tuple is used for finding the last operation
* to occur in an expression 
*/
typedef struct {
    operator_type op_type;
    long depth;
    long prec;
    long assoc;
    long index;
} tuple_t;


/*
* Error interface for AST building and creation. parser.c defines an internal
* variable to store these codes.
*/
typedef enum {
    PARSE_OK,
    PARSE_INVALID_EXPRESSION,
    PARSE_MEMORY_FAILURE,
    PARSE_INVALID_TOKENS,
    PARSE_STATUS_DNE
} parse_status;


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
ast_t *create_ast_from_tokens(const token_t *tokens, size_t sz, parse_status *status);


/*
* Recursively creates an AST from a token_t array `tokens` of `sz` elements. 
* 
* Returns a pointer to the root of the tree upon success. If a failure occurs,
* an error code is written to `status` and a pointer to the root of the tree 
* is returned so the caller can free the partially-built tree (unless the root
* could not be initialized. This is the only case where NULL is returned).
*/
node_t *create_ast_helper(const token_t *tokens, int low, int high);


#endif
