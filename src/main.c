/*
* Entry point
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "token.h"
#include "lexer.h"
#include "errorprinter.h"

/*
* Checks if an error message exists in the errorprinter buffer and prints it. If
* a message from errorprinter was printed, it returns true.
*
* If no message existed in the buffer, false is returned/
*/
static bool print_error_message(void) {
    if (!has_error()) {
        return false;
    }
    fprintf(stderr, "%s\n", get_error());
    return true;
}



int main(int argc, char **argv) {

    /* Input must be a single string */
    if (argc != 2) {
        fprintf(stderr, "Usage: ./%s [expression]\n", argv[0]);
    }

    /* Assume argv[1] is the sole input string */
    char *expr = argv[1];
    
    /* Create tokens array. This array is on the heap and must be freed. */
    tokens_status tok_status = TOKENS_OK;
    size_t token_count = 0;
    token_t *tokens;

    if (!(tokens = create_tokens_from_string(expr, &token_count, &tok_status)) 
        || tok_status != TOKENS_OK) {
        if (!print_error_message()) {
            fprintf(stderr, "Error: Invalid expression. '%s'\n", expr);
        }
        goto FREE_TOKENS_AND_FAIL;
    }

    return EXIT_SUCCESS;

FREE_TOKENS_AND_FAIL:
    fully_free_tokens(tokens, token_count);
    return EXIT_FAILURE;
}
