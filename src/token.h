#ifndef TOKEN_H
#define TOKEN_H

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
} operator_t;


/* Arity, precedence, and operand type tables for operators */
extern const char arity[NUM_OP];
extern const char precedence[NUM_OP];


#endif
