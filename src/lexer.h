#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "matrix.h"

#include <stdlib.h>
#include <stdbool.h>

/*
* Tokens status model. TOKENS_OK means user input was correctly tokenized.
*/
typedef enum {
    TOKENS_OK,
    TOKENS_INVALID_ARG,
    TOKENS_INVALID_MATRIX,
    TOKENS_MEMORY_FAILURE 
} tokens_status;


/*
* Creates a matrix token given the number of entries in the matrix (nrow * ncol). 
* Internally, it calls strtok(NULL, TOKEN_DELIM) nrow*ncol times to consume the
* entries of the matrix. Hence, it should only be called after strtok has been fed 
* an initial string.
*/
tokens_status create_matrix_token(token_t *token, unsigned int nrow, unsigned int ncol);


/*
* Tokenizes 'c' into a LPAREN or RPAREN token. The new token is written to 'dst'.
* A tokens_status code is returned to indicate the success or failure of the tokenization.
*/
tokens_status create_parens_token(const char c, token_t *dst);


/*
* Creates a scalar token given a (valid) scalar value. The new token is written to dst.
*/
tokens_status create_scalar_token(scalar_t scalar, token_t *dst);


/*
* Takes a constant string and produces a token array terminated with a token of type TOKENS_END. 
* It returns a pointer to an TOKENS_END-terminated array of token_t upon sucess and NULL upon 
* failure.
*
* If 'token_count' is not NULL, the number of tokens generated is written there.
*
* If 'status' is not NULL, the status of tokenization is written there. This status
* will be TOKENS_OK if the string was correctly tokenized. Otherwise, the corresponding
* error code is written there.
*/
token_t *create_tokens_from_string(const char *str, size_t *token_count, tokens_status *status);


/*
* Reads a string and writes its corresponding token_t object to 'dst' upon success. TOKENS_OK is 
* returned. If the string cannot be tokenized, the corresponding error status code is returned and nothing
* is written to 'dst'.
*
* This function tokenizes a string into an operator, parenthesis, or scalar. It does not tokenize matrices.
* Matrices are tokenized by create_matrix_token.
*
* The token_t 'obj' pointer points to a heap address and must be freed by the caller.
*/
tokens_status create_token_from_str(const char *str, token_t *dst);


/*
* Returns true if 'str' is a valid operator label. If 'type' is not NULL, the operator type is
* written there.
*/
bool is_operator(const char *str, operator_type *type);


/*
* Returns true if 'str' is a valid scalar. If 'val' is not NULL, the scalar value is written there.
* It is NON-REPORTING should 'str' not be a valid scalar. As such, it does not call str_to_scalar_t,
* which is reporting.
*/
bool is_scalar(const char *str, scalar_t *val);

/*
* Tokens must be separated from each other by at least one of these characters below.
*/
#define TOKEN_DELIM " \t\n\v\f\r"

/*
* The first token array allocated will have this size, and the lexer will enlarge the array if needed.
*
* See create_tokens_from_string to see how it's used.
*/
#define TOKENS_ARR_SIZE 24


#endif
