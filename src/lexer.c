#include "lexer.h"
#include "matrix.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>


/*
* Returns a scalar_t according to 'str' upon success. 
*
* If no conversion is done, 0 is returned and the error is 
* written to 'status' if not NULL.
*/
scalar_t str_to_scalar_t(const char *str, matrix_status *status) {
    if (!str || *str == '\0') {
        goto RETURN_UPON_ERROR;
    }

    char *endptr;
    errno = 0;
    double val = strtod(str, &endptr);

    if (endptr == str || *endptr != '\0' || errno != 0) {
        goto RETURN_UPON_ERROR;
    }
    if (status) { *status = MATRIX_OK; }
    return (scalar_t)val;

RETURN_UPON_ERROR:
    if (status) { *status = MATRIX_INVALID_ENTRY; }
    return 0;
}


/*
* Calls strtok(NULL, TOKEN_DELIM) 'n' times and returns a pointer to a
* heap-allocated array of 'n' scalar_t values upon success.
*
* It calls set_error and returns NULL upon failure. No memory is allocated
* upon failure.
*/
static scalar_t *get_matrix_entries_from_str(int n, tokens_status *status) {
    scalar_t *data = malloc(n*sizeof(scalar_t));
    if (!data) {
        /* TODO: set memory error */
        if (status) { *status = TOKENS_MEMORY_FAILURE; }
        return NULL;
    }
    matrix_status mat_st;
    char *str;
    scalar_t val;
    for (int i = 0; i < n; i++) {
        str = strtok(NULL, TOKEN_DELIM);
        
        /* If strtok returns NULL, not enough entries exist */
        if (!str) {
            /* TODO: set error */
            if (status) { *status = TOKENS_INVALID_MATRIX; }
            goto RETURN_UPON_ERROR;
        }

        val = str_to_scalar_t(str, &mat_st);
        if (mat_st != MATRIX_OK) {
            /* TODO: could set error here */
            if (status) { *status = TOKENS_INVALID_MATRIX; }
            goto RETURN_UPON_ERROR;
        }
        data[i] = val;
    }
    if (status) { *status = TOKENS_OK; }
    return data;

RETURN_UPON_ERROR:
    free(data);
    return NULL;
}


/*
* Returns true if 'str' has the form "axb" where "a" and "b" are positive integers
* and false otherwise.
*/
static bool is_matrix_marker(const char *str, int *nrow, int *ncol) {
    if (!str || *str == '\0' || *str == 'x') {
        return false;
    }
    char *endptr;
    errno = 0;
    unsigned long a = strtoul(str, &endptr, 10);
    
    if (*endptr != 'x' || errno == ERANGE) {
        /* TODO: use a global error message buffer and write a "invalid matrix dimension" message there.*/
        return false;
    }
    
    if (*endptr++ == '\0') {
        return false;
    }

    str = endptr;
    errno = 0;
    unsigned long b = strtoul(str, &endptr, 10);
    if (endptr == str || *endptr != '\0' || errno == ERANGE) {
        return false;
    }

    if (nrow) { *nrow = a; }
    if (ncol) { *ncol = b; }

    return true;
}


/* 
* Returns a pointer to a reallocated memory region twice as big as current_size upon success.
* Returns 'tokens' upon failure so caller can free the memory at this address. Since the return
* value cannot signal a failure, caller MUST check errno == ENOMEM for failure. 
*
* Note that the caller's size variable is updated accordingly.
*/
static token_t *resize_tokens(token_t *tokens, size_t *current_size) {
    if (!tokens || !current_size) {
        return NULL;
    }
    size_t new_size = 2 * (*current_size);
    token_t *tmp = realloc(tokens, new_size);
    if (!tmp) {
        /* Note: memory at 'tokens' is still valid if realloc fails, so caller can free it */
        return tokens;
    }
    *current_size = new_size;
    return tmp;
}



tokens_status create_matrix_token(token_t *token, int nrow, int ncol) {
    if (nrow <= 0 || ncol <= 0 || !token) {
        return TOKENS_INVALID_ARG;
    }
    
    tokens_status status;
    scalar_t *data = get_matrix_entries_from_str(nrow*ncol, &status);
    if (!data || status != TOKENS_OK) {
        return status;
    }

    matrix_t *mat = init_matrix(data, nrow, ncol);
    if (!mat) {
        free(data);
        return TOKENS_MEMORY_FAILURE;
    }
    token->type = MATRIX;
    token->obj = mat;

    return TOKENS_OK;
}


token_t *create_tokens_from_string(const char *str, size_t *token_count, tokens_status *status) {
    if (!str || *str == '\0') {
        return NULL;
    }

    /* Make mutable copy of string */
    char *m_str = strdup(str);
    if (!m_str) {
        if (status) { *status = TOKENS_MEMORY_FAILURE; }
        return NULL;
    }

    /* Allocate token_t array */
    size_t size = TOKENS_ARR_SIZE;
    token_t *tokens = malloc(size*sizeof(token_t));
    if (!tokens) {
        if (status) { *status = TOKENS_MEMORY_FAILURE; }
        free(m_str);
        return NULL;
    }
    
    size_t tc = 0;
    char *tok_str;
    tokens_status st;

    matrix_status mat_st;
    int nrow = -1;
    int ncol = -1;

    /* Consume first token  */
    tok_str = strtok(m_str, TOKEN_DELIM);

    if (is_matrix_marker(tok_str, &nrow, &ncol)) {
        if ((st = create_matrix_token(tokens, nrow, ncol)) != TOKENS_OK) {
            if (status) { *status = st; }
            free(m_str);
            free(tokens);
            return NULL;
        } 
    }
    else if ((st = create_token_from_str(tok_str, tokens)) != TOKENS_OK) { 
        if (status) { *status = st; }
        free(m_str);
        free(tokens);
        return NULL;
    }
    tc++;

    /* Tokenize the rest of the string */
    while ((tok_str = strtok(NULL, TOKEN_DELIM)) != NULL) {

        /* Resize if token count has reached current max size */
        if (tc == size) {
            tokens = resize_tokens(tokens, &size);
            if (errno == ENOMEM) {
                if (status) { *status = TOKENS_MEMORY_FAILURE; }
                goto FREE_TOKENS_UPON_ERROR; 
            }
        }

        /* Check if tok_str is a matrix marker "axb" */
        if (is_matrix_marker(tok_str, &nrow, &ncol)) {

        }

        /* Tokenize the current string at tok_str */
        if ((st = create_token_from_str(tok_str, tokens + tc)) != TOKENS_OK) {
            if (status) { *status = st; }
            goto FREE_TOKENS_UPON_ERROR;
        }
        tc++;
    }

    /* Terminate the tokens array with the marker type TOKENS_END */
    if (tc == size) {
        tokens = resize_tokens(tokens, &size);
        if (errno == ENOMEM) {
            if (status) { *status = TOKENS_MEMORY_FAILURE; }
            goto FREE_TOKENS_UPON_ERROR;
        }
    }
    tokens[tc].type = TOKENS_END;
    tokens[tc].obj = NULL;
    /* We do not increment tc here because tc counts non-END tokens */

    if (token_count) { *token_count = tc; }
    if (status) { *status = TOKENS_OK; }

    return tokens;

FREE_TOKENS_UPON_ERROR:
    free_token_objs_by_count(tokens, tc);
    free(tokens);
    free(m_str);
    return NULL;
}


tokens_status create_token_from_str(const char *str, token_t *dst) {
    if (!str || *str == '\0') {
        return ;
    }
    
    

    return TOKENS_OK;
}

