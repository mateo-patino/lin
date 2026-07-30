#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

/*
* This file defines the basic data types used by this program.
*
* The basic interface for operators and matrices is also defined here.
*/


/*
* Token interface...
*/
typedef enum {
    OPERATOR,
    SCALAR,
    MATRIX,
    LPAREN,
    RPAREN,
    TOKENS_END /* signals the end of a token_t sequence */
} token_type;


typedef struct {
    token_type type;
    void *obj;
} token_t;


/*
* Operator interface...
*/

/* Supported operators */
typedef enum {
    MAT_ADD,
    MAT_SUB,
    MAT_MUL,
    SMUL,
    DET,
    RREF,
    INV,
    NUM_OP
} operator_type;

typedef enum {
    MATRIX_OPERAND,
    SCALAR_OPERAND
} operand_type;


/*
* There is no operator struct because all properties of operators are saved
* in static tables that are parametized by operator_type values. 
*
* Tokens of type OPERATOR have an 'obj' pointer to an operator_type value,
* which can be used to query tables for information about the operator.
*/

typedef enum {
    LEFT_ASSOC,
    RIGHT_ASSOC
} assoc;

/* Arity, precedence, associativity, and alias tables for operators */
extern const char arity[NUM_OP];
extern const char precedence[NUM_OP];
extern const assoc associativity[NUM_OP];
extern const char **operator_alias[NUM_OP];


/*
* Frees 'count' token_t->obj pointers in a 'tokens' array.
* The 'tokens' pointer is not freed, however.
*/
void free_token_objs_by_count(token_t *tokens, size_t count);


/*
* Frees a token_t's obj member (but not the token itself).
*/
void free_token_obj(token_t *tok);

#endif
