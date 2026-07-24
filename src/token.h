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
    INV
} operator_type;


typedef struct {
   operator_type op;
} operator_t;

#endif
