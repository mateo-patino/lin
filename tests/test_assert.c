#include "test_assert.h"
#include "test_helpers.h"
#include "types/matrix.h"

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

