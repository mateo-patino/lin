#ifndef TOKEN_H
#define TOKEN_H

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
    RPAREN
} token_type;


typedef struct {
    token_type type;
    void *obj;
} token_t;


typedef enum {
    MATRIX_T,
    SCALAR_T
} operand_type;


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


typedef struct {
   operator_type op;
   operand_type in_type;
   operand_type out_type;
} operator_t;

typedef enum {
    LEFT_ASSOC,
    RIGHT_ASSOC
} assoc;

/* Arity, precedence, and operand type tables for operators */
extern const char arity[NUM_OP];
extern const char precedence[NUM_OP];
extern const assoc associativity[NUM_OP];


#endif
