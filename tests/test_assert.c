#include "test_assert.h"
#include "test_helpers.h"
#include "types/matrix.h"
#include "types/token.h"

void assert_true_failed(const char *expr, const char *file, int line, const char *func) {
    perr(BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    perr("   Assertion failed: %s\n", expr);
    perr("   at %s:%i\n", file, line);
    perr("   in %s\n", func);
}

void assert_eq_scalar_failed(scalar_t actual, scalar_t expected,
                             const char *actual_expr, const char *expected_expr,
                             const char *file, int line, const char *func) {
    perr(BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    perr("   Assertion failed: %s == %s\n", actual_expr, expected_expr);
    perr("   Actual: " PRISCALAR "\n", 6, actual);
    perr("   Expected: " PRISCALAR "\n", 6, expected);
    perr("   at %s:%i\n", file, line);
    perr("   in %s\n", func);
}


void assert_eq_matdata_failed(const scalar_t *actual, const scalar_t *expected, size_t sz, const char *file, 
                          int line, const char *func) {
    perr(BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    perr("   Expected entries: ");
    print_matrix_data(stderr, actual, sz, true);
    perr("   Actual entries: ");
    print_matrix_data(stderr, expected, sz, true);
    perr("   at %s:%i\n", file, line);
    perr("   in %s\n", func);
}

/* Helper to the ASSERT_EQ_MATDATA macro */ 
bool equal_matrix_data(const scalar_t *actual, const scalar_t *expected, size_t sz) {
    for (size_t i = 0; i < sz; i++) {
        if (!is_close(actual[i], expected[i], 1e-12, 1e-7)) {
            return false;
        }
    }
    return true;
} 


/*
* Returns true if `node` contains an operator of type `op` and false otherwise 
*/
bool assert_operator_node(const node_t *node, operator_type op) {
    if (!node || !node->token || node->token->type != OPERATOR) {
        return false;
    }

    operator_type *op_type = (operator_type *)node->token->obj;
    if (!op_type || *op_type != op) {
        return false;
}
    return true;
}


void assert_operator_node_failed(operator_type op, const char *file, int line, const char *func) {
    perr(BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    perr("   Expected AST node with operator: %s\n", operator_to_str(op));
    perr("   at %s:%i\n", file, line);
    perr("   in %s\n", func);
}


/*
* Returns true if `node` is a node_t pointer with a scalar valued `val`
*/
bool assert_scalar_node(const node_t *node, scalar_t val) {
    if (!node || !node->token || node->token->type != SCALAR) {
        return false;
    }
    scalar_t *scalar = (scalar_t *)node->token->obj;
    if (!scalar || !is_close(*scalar, val, 1e-12, 1e-7)) {
        return false;
    }
    return true;
}


void assert_scalar_node_failed(scalar_t val, const char* file, int line, const char *func) {
    perr(BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    perr("   Expected AST node with scalar: " PRISCALAR "\n", val, 6);
    perr("   at %s:%i\n", file, line);
    perr("   in %s\n", func);
}
