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


#endif
