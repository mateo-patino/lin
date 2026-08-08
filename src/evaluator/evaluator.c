#include "evaluator.h"
#include <stdbool.h>


/*
* An internal interface for recording error codes.
*/
static eval_status internal_eval_status = EVAL_OK;
static bool has_error_status = false;

static void set_status(eval_status code) {
    /* If an error status (!= EVAL_OK) has been set, don't overwrite */
    if (internal_eval_status != EVAL_OK) {
        return;
    }
    internal_eval_status = code;
    if (code != EVAL_OK) {
        has_error_status = true;
    }
}


static eval_status get_status(void) {
    return internal_eval_status;
}


/* A common pattern is to set a status and return NULL */
#define RETURN_NULL_AND_STATUS(x) \
    do { \
        set_status(x); \
        return NULL; \
    } while (0)


result_t *evaluate_ast(const ast_t *ast, eval_status *status) {
    if (!ast || !ast->root) {
        RETURN_NULL_AND_STATUS(EVAL_INVALID_AST);
    }

    /* Set internal status to OK before starting */
    set_status(EVAL_OK);

    /* Perform pre-evaluation semantic checks TODO */

    /* TODO: evaluation requires a lot of intermediate results.
    * These results need to live in the heap, and malloc'ing
    * freeing everything will be very tedious. 
    *
    * We will need to an arena allocator
    */

}
