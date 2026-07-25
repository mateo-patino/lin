#include "lexer.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

/*
* Returns true if 'str' has the form "axb" where "a" and "b" are positive integers
* and false otherwise.
*/
static bool is_matrix_marker(const char *str) {
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

    /* Consume first token */
    tok_str = strtok(m_str, TOKEN_DELIM);
    if ((st = create_token_from_str(tok_str, tokens)) != TOKENS_OK) { 
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
        if (is_matrix_marker())

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

